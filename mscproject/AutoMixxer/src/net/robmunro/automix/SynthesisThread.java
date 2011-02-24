package net.robmunro.automix;
/** ********************************************************************************
 * SynthesisThread extends Thread
 * @author robm
 *
 */
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.util.Vector;

import android.app.Activity;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

public class SynthesisThread extends Thread {
	public static final int BLOCK_SIZE = 8192;
	public static final int SAMPLE_RATE = 22050; 
	
	float overlapSecs = 10;
	AutoMixActivity act;
	AudioTrack oTrack;
	
	Vector<SongDataBean> fileList = null; 
	int playIndex = -1; 
	int nextIndex = -1;
	SongDataBean thisSong = null; 
	SongDataBean nextSong = null;
	
	ShortBuffer theBuffer;
	
	boolean stop = false;
	public boolean isMixing = false;
	
	boolean ffToMix = false;
	public SynthesisThread(AutoMixActivity act,Vector<SongDataBean> fileList, int startIndex) {
		this.act=act;
		this.fileList=fileList;
		playIndex=startIndex;
		initFields(SongDataBean.class);
	}

	@Override
	public void run() {
		// setup
		AudioTrack oTrack = new AudioTrack(AudioManager.STREAM_MUSIC, SAMPLE_RATE, 
                AudioFormat.CHANNEL_CONFIGURATION_MONO,  
                AudioFormat.ENCODING_PCM_16BIT, BLOCK_SIZE,
                AudioTrack.MODE_STREAM);
		oTrack.play();  
		
		if (fileList!=null && fileList.size()>0) {
			playIndex = ++playIndex%fileList.size();  
			nextIndex=(playIndex+1)%fileList.size();
		}
		
		if (playIndex>-1 && playIndex<fileList.size()) { 
			thisSong = fileList.get(playIndex);
			openMP3File(thisSong,thisSong.path);
			thisSong.playState=SongDataBean.PLAYSTATE_QUEUED;
			act.thisSongDisp.setSong(thisSong);
			thisSong.vol=0;
		} 
		//SongDataBean nextSong = null;
		if (nextIndex>-1 && nextIndex<fileList.size()) {
			queueNextSong(nextIndex);
		}
		short[] samples = new short[BLOCK_SIZE];
		short[] samples1 = new short[BLOCK_SIZE]; 
		if (theBuffer==null) {
			 ByteBuffer byteBuffer = ByteBuffer.allocateDirect( samples.length * Short.SIZE / 8 );
	         byteBuffer.order(ByteOrder.nativeOrder());
	         theBuffer = byteBuffer.asShortBuffer();
		}
		 
		if (thisSong!=null || nextSong!=null) {
			setupPlay(BLOCK_SIZE);
			thisSong.playState=SongDataBean.PLAYSTATE_PLAYING;
			int readSamples = readSamples( thisSong, nextSong, theBuffer); //, samples.length    
			WriterThread wt = new WriterThread(oTrack,BLOCK_SIZE);
	    	wt.samps=samples1;
	    	wt.start(); 
			while (readSamples>0 && !stop) {
				long start = System.currentTimeMillis();
				theBuffer.position(0);
				theBuffer.get( samples );   
				//oTrack.write(samples, 0, readSamples);
				long write = System.currentTimeMillis();
				readSamples = readSamples( thisSong, nextSong, theBuffer); //, samples.length 
				Log.d(AutoMixActivity.TAG,"read:"+(write-System.currentTimeMillis()));
	    		if (ffToMix) {
	    			while (thisSong.getRemaining() > act.overlapSecs+10) {  
	    				readSamples = readSamples( thisSong, nextSong, theBuffer);
	    				theBuffer.position(0);  
	    				theBuffer.get( samples );   
	    			}
	    			ffToMix=false;
	    		}
				while (!wt.written) {try {Thread.sleep(10);} catch (InterruptedException e) { 	} }
				samples = wt.swap(samples);
				//oTrack.write(samps, 0, nread); 
	    		wt.written=false;
				
	    		
				int remaining = (int)(thisSong.dur/1000 - thisSong.playPosition/thisSong.sampleRate);
				Log.d(AutoMixActivity.TAG,(write-System.currentTimeMillis())+": remain:"+remaining+" nssc:"+nextSong.scaleFactor);   
				if (remaining<=act.overlapSecs && nextSong.playState!=SongDataBean.PLAYSTATE_PLAYING_SCALED) {
					isMixing=true;
					//Log.d(AutoMixActivity.TAG,"mix");   
					nextSong.playState=SongDataBean.PLAYSTATE_PLAYING_SCALED;
					nextSong.scaleFactor = nextSong.tempo/thisSong.tempo;
				} else if (remaining<=0 && nextSong.scaleFactor!=1) {
					Log.d(AutoMixActivity.TAG,"restore");   
					thisSong.playState=SongDataBean.PLAYSTATE_STOPPED;
					float ts = nextSong.scaleFactor;
					ts = ts+(1-ts)*0.03f;
					if (ts>1) {ts-=0.001;}
					if (ts<1) {ts+=0.001;}
					if (ts<1.001 && ts>0.999) {ts=1;}
					nextSong.scaleFactor=ts;
				} else if (remaining<=0) {
					
					//Log.d(AutoMixActivity.TAG,"switch");   
					closeMP3File(thisSong); 
					nextSong.playState=SongDataBean.PLAYSTATE_PLAYING;
					thisSong=nextSong;
					nextSong=null;
					act.thisSongDisp.setSong(thisSong);
					playIndex=nextIndex;
					nextIndex=(playIndex+1)%fileList.size();
					queueNextSong(nextIndex);
					isMixing=false;
				}
			}
			wt.stop=true;
			tearDownPlay();
		}
		
		// teardown
		oTrack.flush(); 
		oTrack.stop();
		oTrack.release();
	}
	public void ffToMix() {
		ffToMix=true;
	}  
	public boolean queueNextSong(int index) {
		if (index<fileList.size() && (nextSong==null || nextSong.playState==SongDataBean.PLAYSTATE_QUEUED)) {
			if (nextSong!=null) {closeMP3File(nextSong); }
			nextIndex = index;
			nextSong = fileList.get(nextIndex);
			nextSong.vol=nextSong.peakVol/thisSong.peakVol;
			nextSong.playState=SongDataBean.PLAYSTATE_QUEUED;
			//
			float diff = Math.abs(thisSong.tempo-nextSong.tempo);
			if (Math.abs(thisSong.tempo-nextSong.tempo*2)<diff) {
				nextSong.scaleFactor = nextSong.tempo*2/thisSong.tempo;
			} else if (Math.abs(thisSong.tempo-nextSong.tempo/2)<diff) {
				nextSong.scaleFactor = nextSong.tempo/2/thisSong.tempo;
			} else {
				nextSong.scaleFactor = nextSong.tempo/thisSong.tempo;
			}
			openMP3File(nextSong,nextSong.path); 
			act.nextSongDisp.setSong(nextSong);
			return true;
		}
		return false;
	}
	
	class WriterThread extends Thread{
    	boolean written = true;
    	boolean stop = false;
    	AudioTrack at = null;
    	short[] samps;
    	int size;
    	public WriterThread(AudioTrack at, int size) { 
    		this.at=at;
    		this.size = size;    
    	}
    	public void run() {
    		while (!stop) {
    			long time = System.currentTimeMillis();
	    		at.write(samps, 0, size);  
	    		Log.e("p1j","write:"+(System.currentTimeMillis()-time)); 
	    		written=true;
	    		while (written) {
	    			try {
						Thread.sleep(10);
					} catch (InterruptedException e) {
						
					}
	    		}
    		}
    	}
    	public short[] swap(short[] samples) {
    		short[] tmp = samps;
    		samps=samples;
    		return tmp;
    	}
    }
	public native int initFields(Class<SongDataBean> songData);
	public native int readSamples( SongDataBean thisSong,  SongDataBean nextSong, ShortBuffer buffer ); //, int size
	public native int openMP3File( SongDataBean thisSong, String path);
	public native int closeMP3File( SongDataBean thisSong);
	public static native int setupPlay(int blockSize ); 
	public static native int tearDownPlay(  );

	
}
