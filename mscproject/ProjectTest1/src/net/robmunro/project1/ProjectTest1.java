package net.robmunro.project1;

import java.io.File;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Process;
import android.util.Log;
import android.view.View;
import android.view.View.MeasureSpec;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.SeekBar.OnSeekBarChangeListener;

public class ProjectTest1 extends Activity {
    /** Called when the activity is first created. */
	private static final int SAMPLE_RATE=22050;
	private static final String PREFS="mscproj";
	private static final String PREF_INPUT="input";
	private static final String PREF_SF="sf";
	private static final String PREF_CD="cd";
	private static final String PREF_WPD="wpd"; 
	boolean isRunning=false; 
	boolean cancel=false;   
	TextView td =null;
	String tdText="new";
	int fftSize = 256; 
	short[] fftData = new short[fftSize];
	GraphView fftGraph; 
	BarView sfBar;
	SeekBar sfseek;
	BarView cdBar;
	SeekBar cdseek;
	BarView wpdBar;  
	SeekBar wpdseek;
	SeekBar inputseek;
	TextView inputScalingText;
	TextView tempoText;
	float[] tempo=new float[2];
	int sf = 0;
	int cd = 0;
	int wpd = 0;
	AudioRecord mRecord;
	AudioTrack oTrack;
	SharedPreferences settings;
	//boolean stopPlayingMP3=false;   
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main); 
        setTitle("Onset testing");
        td = (TextView)findViewById(R.id.testDisp);   
         settings=this.getSharedPreferences(PREFS, 0);  
         
        sfBar=new BarView(this,100,"sf");
        ((FrameLayout)findViewById(R.id.sfFrame)).addView(sfBar,new FrameLayout.LayoutParams(FrameLayout.LayoutParams.FILL_PARENT,20));
        sfBar.setLevel(0);
        sfBar.setColour(175, 125, 0);
        sfseek=(SeekBar)findViewById(R.id.sf_thresh);
        sfseek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				sfBar.setThreshold(progress);
				
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
        });
        sfseek.setProgress(settings.getInt(PREF_SF, 100));
        cdBar=new BarView(this,500,"cd");
        ((FrameLayout)findViewById(R.id.cdFrame)).addView(cdBar,new FrameLayout.LayoutParams(FrameLayout.LayoutParams.FILL_PARENT,20));
        cdBar.setLevel(0);
        cdBar.setColour(255, 175, 0);
        cdseek=(SeekBar)findViewById(R.id.cd_thresh);
        cdseek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				cdBar.setThreshold(progress);
			} 
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
        });
        cdseek.setProgress(settings.getInt(PREF_CD, 100));
        
        wpdBar=new BarView(this,1,"wpd");
        ((FrameLayout)findViewById(R.id.wpdFrame)).addView(wpdBar,new FrameLayout.LayoutParams(FrameLayout.LayoutParams.FILL_PARENT,20));
        wpdBar.setLevel(0);
        wpdBar.setColour(0, 200, 0);
        wpdseek=(SeekBar)findViewById(R.id.wpd_thresh);
        wpdseek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				wpdBar.setThreshold(progress);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
        });
        wpdseek.setProgress(settings.getInt(PREF_WPD, 100));
        inputScalingText = (TextView)findViewById(R.id.input_scaling_text);
        inputseek=(SeekBar)findViewById(R.id.input_scaling);
        inputseek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				setInputScale(progress);
				inputScalingText.setText("input scaling :"+progress);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
        });
        inputseek.setProgress(settings.getInt(PREF_INPUT, 10));
        inputScalingText.setText("input scaling :"+inputseek.getProgress()); 
        
        fftGraph = new GraphView(this);
        FrameLayout fl = (FrameLayout)findViewById(R.id.testGraphView);
        fl.addView(fftGraph,new FrameLayout.LayoutParams(300,200));
        
        tempoText = (TextView)findViewById(R.id.tempo_text);
        
        Button b1 = (Button)findViewById(R.id.testPassthru);
        b1.setOnClickListener(new OnClickListener() {  
			@Override
			public void onClick(View v) {
				if (!isRunning) {
					cancel=false;
					Thread t=(new Thread(){
						@Override
						public void run() {
							isRunning=true;
							try {
								createObjects();
								passThruNativeDirect();
							} catch (Exception e) {
								Log.e("pj1", e.getMessage(),e);
							} 
							//removeObjects();
							isRunning=false;
						}
						
					});
					t.setPriority(Thread.MAX_PRIORITY);
					t.start();
					
				} else {
					cancel=true;
				}
			} 
		});
        Button b2 = (Button)findViewById(R.id.testMP3);
        b2.setOnClickListener(new OnClickListener() {  
			@Override
			public void onClick(View v) { 
				if (!isRunning) {
					cancel=false;
					Thread t=(new Thread(){
  
						@Override
						public void run() {
							try {
								isRunning=true;
								playMP3File();
							} catch (Exception e) {
								Log.e("pj1", e.getMessage(),e); 
							} 
							isRunning=false;
						}
					});
					t.setPriority(Thread.MAX_PRIORITY);
					t.start();
					
				} else {
					cancel=true;
				}
			}
		});
        Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO); 
    }
      
    @Override
	protected void onStop() {
		super.onStop();
		cancel=true;
		if (mRecord!=null ){
			try {
				if (mRecord.getRecordingState()==AudioRecord.RECORDSTATE_RECORDING) {
					mRecord.stop();
				}
				//if (mRecord.getRState()==AudioRecord.STATE_INITIALIZED) {
				mRecord.release();
				mRecord=null;
			} catch (Exception e) {
				Log.e("PJ1","onStop recorder:"+e.getMessage(),e);
			}
		}
		if (oTrack!=null) { 
			try {
				if (oTrack.getPlayState()!=AudioTrack.PLAYSTATE_STOPPED) {
					oTrack.stop();
				}  
				oTrack.release();
				oTrack=null;
			} catch (Exception e) {
				Log.e("PJ1","onStop player:"+e.getMessage(),e);
			}
		}  
		// save prefs
		Editor edit = settings.edit();
		edit.putInt(PREF_SF, sfseek.getProgress());
		edit.putInt(PREF_CD, cdseek.getProgress());
		edit.putInt(PREF_WPD, wpdseek.getProgress());
		edit.putInt(PREF_INPUT, inputseek.getProgress());
		edit.commit();
	} 
    
    private void passThruNativeDirect() {
    	int rintSize = AudioRecord.getMinBufferSize(SAMPLE_RATE,
                AudioFormat.CHANNEL_CONFIGURATION_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
    	
    	mRecord = new AudioRecord(
	    		MediaRecorder.AudioSource.MIC,
	    		SAMPLE_RATE,
	    		AudioFormat.CHANNEL_CONFIGURATION_DEFAULT,
	    		AudioFormat.ENCODING_DEFAULT,
	    		rintSize*4
	    );  
    	
    	int tintSize = AudioTrack.getMinBufferSize(SAMPLE_RATE,
                AudioFormat.CHANNEL_CONFIGURATION_MONO,
                AudioFormat.ENCODING_PCM_16BIT);
    	 
	    oTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 
	    									SAMPLE_RATE,
	    									AudioFormat.CHANNEL_CONFIGURATION_MONO,
	    									AudioFormat.ENCODING_PCM_16BIT, 
	    									tintSize,
	    									AudioTrack.MODE_STREAM);
	    
	    if (mRecord.getState()==AudioRecord.STATE_INITIALIZED) {
	    	mRecord.startRecording();
		    oTrack.play();
		    if (oTrack!=null && mRecord!=null) {
		    	try {
			    	//byte[] byteData = new byte[10000];
			    	int bytesRead=-1; 
			    	isRunning=true;
			    	cancel=false;
			    	ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rintSize);
			    	ByteBuffer obyteBuffer = ByteBuffer.allocateDirect(rintSize);
		 	        while((bytesRead=mRecord.read( byteBuffer,rintSize ))>-1 && !cancel) {
					    // Write the byte array to the track
		 	        	//processBuffer(byteBuffer,obyteBuffer,bytesRead);
		 	        	processBuffer2(byteBuffer,obyteBuffer,bytesRead);
		 	        	byte[] byteData = new byte[bytesRead];
		 	        	int i= obyteBuffer.remaining();
		 	        	obyteBuffer.get( byteData,0,i);  
		 	        	// testing comment out passback 
				    	//oTrack.write(byteData,0,i);//output.array() - DirectByteBuffer is protected
				    	//obyteBuffer.clear();
				    	//byteBuffer.clear();  
				    	  
				    	obyteBuffer.rewind(); 
				    	byteBuffer.rewind();   
				    	 if (lastTime<System.currentTimeMillis()-50) { 
				     	   	//Log.d("PJ!","Get fft data:"+lastTime);
				     	   	getFFTData(fftData, fftSize); 
				     	   	//sf=sf();
				     	   	//cd=cd();
				     	    //wpd=wpd();
				     	   	ProjectTest1.this.runOnUiThread(new Runnable(){
				 				@Override
				 				public void run() {
				 					//fftGraph.requestLayout(); 
				 		    	   	fftGraph.invalidate();
				 		    	   //	sfBar.setLevel(sf);
				 		    	   //	cdBar.setLevel(cd);
				 		    	   //	wpdBar.setLevel(wpd);
				 				}
				     	   	});
				        	lastTime=System.currentTimeMillis();
				        }
				    	//Log.e("pj1", "read:"+bytesRead+": buffsize:"+rintSize);  
		 	        } 
		    	} catch (Exception e) {
		    		Log.e("at1", "Passthru Exception:"+e.getMessage());
			        e.printStackTrace();
			    }
		    } 
		    mRecord.stop();
		    mRecord.release();
	    } else { 
	    	this.runOnUiThread(new Runnable() { public void run() {Toast.makeText(ProjectTest1.this, "record not initialised", 500).show();}});
	    }
	    // Done writing to the track
	    oTrack.flush(); 
	    oTrack.stop();
	    oTrack.release();
    }
    ////////////////////////////////// libMAD testing //////////////////////////////////////
    ShortBuffer buffermp3;
    int handle1 =-1;
    int handle2 =-1; 
    Runnable updater = new Runnable() {public void run(){td.setText(tdText);}};
    private void playMP3File() { 
    	int blockSize = 8192;
    	Log.d("pj1","b4 audiotrack:");      
    	AudioTrack oTrack = new AudioTrack(AudioManager.STREAM_MUSIC, 
	    		SAMPLE_RATE, 
                AudioFormat.CHANNEL_CONFIGURATION_MONO,  
                AudioFormat.ENCODING_PCM_16BIT, blockSize,
                AudioTrack.MODE_STREAM);
    	Log.d("pj1","opened audiotrack:");
    	oTrack.play();  
    	Log.d("pj1","played audiotrack:");
    	//String path1 = "/sdcard/test.mp3"; 
    	//String path2 = "/sdcard/test1.mp3"; 
    	String path1 = "/sdcard/Gorillaz_Feel_Good-_Demon_Days.mp3";
    	String path2 = "/sdcard/shadow_Fixed_Income.mp3";
    	//String path = "/sdcard/sintest2.mp3";  
    	//String opath = "/sdcard/test.pcm";    
    	File f = new File(path1); 
    	//FileWriter fw = new FileWriter(f);
    	Log.d("at1","exists:"+f.exists()); 
    	f=null;            
		handle1=openFile(path1); 
    	Log.d("at1","opened:"+handle1);
    	handle2=openFile(path2); 
    	Log.d("at1","opened:"+handle2);
    	int ctr=0;
    	int bytesDecoded = 0; 
    	if (handle1>-1 && handle2>-1) {
    		createObjects();
	    	short[] samps=new short[blockSize];
	    	short[] samps1=new short[blockSize];  
	    	short[] tmpSamp=null;
	    	//WriterThread wt = new WriterThread(oTrack,blockSize);
	    	//wt.samps=samps1;
	    	//wt.start();
	    	int nread=1;
	    	cancel=false;    
	    	//ByteBuffer byteBuffer = ByteBuffer.allocateDirect( fftSize * Short.SIZE / 8 );
	        //byteBuffer.order(ByteOrder.nativeOrder());
	        //fftBuffer = byteBuffer.asShortBuffer();
	    	//setFFTData(fftBuffer,fftSize); 
	    	long time = System.currentTimeMillis(); 
	    	while ((nread)>0 && !cancel) { 
	    		time = System.currentTimeMillis();
	    		nread  = readSamples(samps);
	    		Log.e("p1j","gen: "+(System.currentTimeMillis()-time)); time = System.currentTimeMillis();
	    		oTrack.write(samps, 0, nread);  
	    		Log.e("p1j","write: "+(System.currentTimeMillis()-time)); 
	    		/*
	    		while (!wt.written) {
	    			try {
						Thread.sleep(10);
					} catch (InterruptedException e) { 	} 
	    		}
	    		tmpSamp = wt.samps;
				wt.samps = samps;
				samps = tmpSamp;
	    		//oTrack.write(samps, 0, nread); 
	    		wt.written=false;
	    		
	    		bytesDecoded+=nread;
	    		if (ctr++%100==0) {
	    			//tdText = oTrack.getPlaybackHeadPosition()+"/"+bytesDecoded; 
	    			//runOnUiThread(updater);
	    		}
	    		*/
	    		
	    	}
	    	//wt.stop=true;
	    	closeFile(handle1);
	    	closeFile(handle2);
	    	removeObjects();
    	} 
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
    }
     
    long lastTime = 0;
    // for reading MP3 file samples.
    public int readSamples(short[] samples)  {
       if( buffermp3 == null || buffermp3.capacity() != samples.length ) {
          ByteBuffer byteBuffer = ByteBuffer.allocateDirect( samples.length * Short.SIZE / 8 );
          byteBuffer.order(ByteOrder.nativeOrder());
          buffermp3 = byteBuffer.asShortBuffer();
       }
      
       int readSamples = readSamples( handle1,handle2, buffermp3, samples.length ); 
       if( readSamples == 0 ) {
          closeFile( handle1 );
          closeFile( handle2 );
          return 0;
       }
     
       buffermp3.position(0);
       buffermp3.get( samples );   
       
       if (lastTime<System.currentTimeMillis()-50) { 
    	   	//getFFTData(fftData, fftSize);
    	   	//sf=sf();
     	   	//cd=cd();
     	   // wpd=wpd();
    	   	tempo[0] = getTempo(0);
    	   	tempo[1] = getTempo(1);
    	   	ProjectTest1.this.runOnUiThread(new Runnable(){
				@Override
				public void run() {
					//fftGraph.invalidate();
					//sfBar.setLevel(sf);
 		    	   //	cdBar.setLevel(cd);
 		    	   //	wpdBar.setLevel(wpd);  
					tempoText.setText("tempo1:"+tempo[0]+" tempo2:"+tempo[1]);
				}
    	   	});
       		lastTime=System.currentTimeMillis();
       }
       
       return samples.length; 
    } 
    
	 
    public class GraphView extends View{
    	Paint fPaint;
    	Paint tPaint;
    	public GraphView(Context context) {
			super(context);
			this.setBackgroundColor(R.drawable.default_background); 
			this.fPaint = new Paint();
			this.fPaint.setARGB(150, 255, 0, 0);
			this.tPaint = new Paint();
			this.tPaint.setARGB(150, 255, 255, 255);
			this.tPaint.setTextSize(12); 
			this.tPaint.setFakeBoldText(true); 
		}
		  
		@Override
		protected void onDraw(Canvas canvas) { 
			super.onDraw(canvas);
			if (fftData!=null) {
				int min = 2000; int max = 0;
				for (int i=0;i<fftSize/2;i++) {
					if (min>fftData[i]) {min = fftData[i];}  
					if (max<fftData[i]) {max = fftData[i];}   
					int y=fftData[i]/20;  
					canvas.drawLine(i, getMeasuredHeight(), i, getMeasuredHeight()-y , this.fPaint);
				}
				canvas.drawText("min:"+min+" max:"+max, 2,10, this.tPaint);
			}
		}

		@Override
		protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) { 
			int wspecMode = MeasureSpec.getMode(widthMeasureSpec);
	        int wspecSize = MeasureSpec.getSize(widthMeasureSpec);
	        
	        int hspecMode = MeasureSpec.getMode(heightMeasureSpec);
	        int hspecSize = MeasureSpec.getSize(heightMeasureSpec);
	        Log.d("PJ1","Measure:"+wspecSize+":"+hspecSize);
			 
			setMeasuredDimension(wspecSize, hspecSize); 
		} 
    }
    
    public class BarView extends View{  
    	private static final int MEAN_BUFFLEN = 20;
		Paint fPaint;
    	Paint tPaint;
    	Paint rPaint;
    	int level = 0;
    	float scale = 0;
    	String label ="bar";
    	int threshold = 100000;
    	int[] meanBuffer=new int[20];
    	int meanBufferPtr = 0;
    	int mean = 0;
    	public BarView(Context context,float scale,String label) {
			super(context);
			this.label=label;
			this.scale=scale;
			this.setBackgroundColor(R.drawable.default_background);
			this.fPaint = new Paint();
			this.fPaint.setARGB(255, 255, 255, 0);
			this.rPaint = new Paint();
			this.rPaint.setARGB(255, 255, 0, 0);
			this.tPaint = new Paint();
			this.tPaint.setARGB(255, 255, 255, 255); 
			this.tPaint.setTextSize(12);
			this.tPaint.setFakeBoldText(true); 
		}
    	
    	@Override
		protected void onDraw(Canvas canvas) { 
			super.onDraw(canvas);
			float val = level/scale;
			if (val > getMeasuredWidth()) {
				val = getMeasuredWidth();  
				tPaint.setARGB(255, 255, 0,0); 
			} else {
				tPaint.setARGB(255, 255, 255,255); 
			}
			canvas.drawRect(0, 0,val , getMeasuredHeight() , this.fPaint);
			canvas.drawText(label+": ("+threshold+") "+level, 2,10, this.tPaint);
			canvas.drawLine(threshold/(float)scale, 0,threshold/(float)scale , getMeasuredHeight() , this.rPaint);
			canvas.drawLine(mean/(float)scale, 0,mean/(float)scale , getMeasuredHeight() , this.tPaint);
			if (level>threshold) {
				canvas.drawRect(getMeasuredWidth()-getMeasuredHeight(), 1,getMeasuredWidth()-1 , getMeasuredHeight()-1 , this.rPaint);
			}
			
		}
    	
    	@Override
		protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
			int wspecMode = MeasureSpec.getMode(widthMeasureSpec);
	        int wspecSize = MeasureSpec.getSize(widthMeasureSpec);
	        
	        int hspecMode = MeasureSpec.getMode(heightMeasureSpec);
	        int hspecSize = MeasureSpec.getSize(heightMeasureSpec);
	        Log.d("PJ1","Bar Measure:"+wspecSize+":"+hspecSize);
			
			setMeasuredDimension(wspecSize, hspecSize); 
		}
    	void setLevel(int level)  {
    		this.level=level; 
    		meanBuffer[meanBufferPtr]=level;
    		meanBufferPtr++;
    		meanBufferPtr = meanBufferPtr%BarView.MEAN_BUFFLEN;
    		mean=0;
			for (int i=0;i<20;i++) {
				mean+=meanBuffer[i];
			}
			mean/=MEAN_BUFFLEN;
    		invalidate();
    	}
    	void setLabel(String s) {
    		label=s;
    		invalidate();
    	}
    	void setThreshold(int thresh) {
    		threshold=Math.round(thresh*scale);
    		invalidate();
    	}
    	void setColour(int r,int g,int b) {
    		this.fPaint.setARGB(255, r, g, b); 
    		invalidate();   
    	} 
    }  
    
    public native void processBuffer2( ByteBuffer input,ByteBuffer output,int bytesRead );
    // mp3 methods  
    public native int openFile( String file);
    public native int readSamples( int handle1, int handle2, ShortBuffer buffer, int size ); 
    //public native void setFFTData(  ShortBuffer buffer, int size );
    public native void closeFile( int handle );
    public native void createObjects( );
    public native void removeObjects( ); 
    public native int getFFTData(short[] buffer,int size);
    public native void setInputScale(float val);
    public native float  getTempo(int idx);
    //public native void runTests();
    // load the native library
    static {
        System.loadLibrary("pj1");
    }
	
}