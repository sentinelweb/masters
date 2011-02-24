package net.robmunro.automix;
/** ********************************************************************************
 * AutoMixActivity extends Activity
 * @author robm
 *
 */
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.math.BigDecimal;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.ShortBuffer;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Vector;


import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.View.OnLongClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.SeekBar.OnSeekBarChangeListener;
 
public class AutoMixActivity extends Activity {
	public static final String TAG = "AutoMixxer-J";
	
	static {
        System.loadLibrary("automix"); 
    }
	static final String BASE = "/sdcard/test"; 
	static final String APP_BASE = "/sdcard/AutoMix";
	File path = null;
	AnalysisThread analysisThread = null;
	SynthesisThread synthesisThread = null;
	String[][] filesx = {  
			{"duel_of_the_iron_mic","02-gza-duel_of_the_iron_mic-www.bizzydownload.tk.mp3"},
			{"da_funk","02-daft_punk-da_funk.mp3"},
			{"Get Rhythm","04 Get Rhythm [Mono Version].mp3"},
			{"Night Train","1-13 Night Train.mp3"},
			{"I Want I Want","03 - I Want I Want.mp3"}, 
			{"So Real","05 - So Real.mp3"},
			{"Feel Good","Gorillaz_Feel_Good-_Demon_Days.mp3"},
			{"Fixed_Income","shadow_Fixed_Income.mp3"}
	};
	Vector<SongDataBean> fileList = new Vector<SongDataBean>();
	
	Timer uiTimer;
	UiUpdater uiUpdater = null;
	
	int width=320;
	int height=480;
	
	// UI Elements
	Button playBut = null;
	Button stopBut = null;
	public SongDispElements thisSongDisp;
	public SongDispElements nextSongDisp;
	SeekBar overlapSeek;
	TextView overlapText;
	public int overlapSecs=10;
	int optIndex=-1;
	int startIndex=0;
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) { 
        super.onCreate(savedInstanceState);
        path = new File(APP_BASE);
        if (!path.exists()) {  	path.mkdirs(); }
        setContentView(R.layout.main);
        LinearLayout filesCtnr = (LinearLayout)findViewById(R.id.file_list);  
        File baseDir = new File(BASE);
        File[] list =  baseDir.listFiles();
        Vector<File> fileVec= new Vector<File>();
        for (int i=0;i<list.length;i++) {
        	File f = list[i];
        	if (f.exists() && f.getName().toLowerCase().endsWith(".mp3")) {
        		fileVec.add(f);
        	} else if (f.isDirectory()) {
        		File[] sublist =  f.listFiles();
        		for (int j=0;j<sublist.length;j++) {
        			File f1 = sublist[j];
	        		if (f1.exists() && f1.getName().toLowerCase().endsWith(".mp3")) {
	        			fileVec.add(f1);
	        		}
        		}
        	}
        }
        for (int i=0;i<fileVec.size();i++) {
        	File f = fileVec.get(i);
        	
        	SongDataBean s = new SongDataBean();
        	s.disp= f.getName();
        	s.file =f;
        	s.path = f.getAbsolutePath();
        	FrameLayout fl  = new FrameLayout(this);
        	fl.inflate(this, R.layout.file_layout, fl);
        	filesCtnr.addView(fl);
        	s.view=fl;
        	((TextView)fl.findViewById(R.id.file_name)).setText(s.disp);
        	((TextView)fl.findViewById(R.id.file_size)).setText(f.length()/1024/1024+"Mb");
        	fl.setOnLongClickListener(new OnLongClickListener() {
				@Override
				public boolean onLongClick(View v) {
					for (int i=0;i<fileList.size();i++) {
						if (fileList.get(i).view==v) {
							optIndex=i;break;
						}
					}
					showDialog(1);
					return false;
				}
        	});
        	Uri media = android.provider.MediaStore.Audio.Media.EXTERNAL_CONTENT_URI; // uri to sd-card
        	String[] projection = {                 
                         MediaStore.Audio.Media._ID,             // 0
                         MediaStore.Audio.Media.ARTIST,          // 1
                         MediaStore.Audio.Media.TITLE,           // 2
                         MediaStore.Audio.Media.DATA,            // 3
                         MediaStore.Audio.Media.DISPLAY_NAME,    // 4
                         MediaStore.Audio.Media.DURATION};		 // 5 
        	String selection = MediaStore.Audio.Media.DISPLAY_NAME + " == '"+f.getName().replaceAll("'", "''")+"'";
        	Cursor cursor = this.managedQuery(media, projection, selection, null, null);
        	while(cursor.moveToNext()){
        		s.dur = cursor.getInt(5);
        		((TextView)fl.findViewById(R.id.file_length)).setText(  s.dur/1000 +"s");
        		Log.d(TAG,cursor.getString(4)+":"+cursor.getString(5));
        	}
        	cursor.close();
        	fileList.add(s);    
        }
       
        thisSongDisp=new SongDispElements(R.id.this_fname,R.id.this_tempo_txt,R.id.this_length_txt,R.id.this_vol_txt,R.id.this_prg,R.id.this_vol,-1);
    	nextSongDisp=new SongDispElements(R.id.next_fname,R.id.next_tempo_txt,R.id.next_length_txt,R.id.next_vol_txt,R.id.next_prg,R.id.next_vol,R.id.next_stretch_ratio);
    	
        playBut = (Button)findViewById(R.id.play);
        playBut.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (synthesisThread==null) {
					synthesisThread = new SynthesisThread(AutoMixActivity.this,fileList,startIndex);
					synthesisThread.start();
				} 
			}
        });
        stopBut = (Button)findViewById(R.id.stop);
        stopBut.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (synthesisThread!=null ) {
					synthesisThread.stop=true;
					synthesisThread=null;
				}
			}
        });
        overlapText = (TextView)findViewById(R.id.overlap_txt);
    	overlapText.setText("Overlap: "+overlapSecs); 
    	
        overlapSeek = (SeekBar)findViewById(R.id.overlap_size);
    	overlapSeek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
				overlapSecs=progress;
				overlapText.setText("Overlap: "+progress);
			}
			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {	}
			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {}
		});
    	overlapSeek.setProgress(overlapSecs);
        
        analysisThread = new AnalysisThread(this,fileList); 
        analysisThread.start();  
        
        Button analyseBut = (Button)findViewById(R.id.analyse);
        analyseBut.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (analysisThread == null || analysisThread.finished) {
					analysisThread = new AnalysisThread(AutoMixActivity.this,fileList); 
			        analysisThread.start();  
				} else {
					analysisThread.finishAfterNext=true;
					Toast.makeText(AutoMixActivity.this, "Analysis will finish after this file", 500).show();
				}
			} 
        });
        analyseBut.setOnLongClickListener(new OnLongClickListener() {
			@Override
			public boolean onLongClick(View v) { 
				for (int i=0;i<fileList.size();i++) {
					if (AnalysisThread.summaryFile!=null) {
						if (AnalysisThread.summaryFile.exists()) {
							AnalysisThread.summaryFile.delete();
						}
					}
					if (fileList.get(i).analysed) {
						File analysisFile = AnalysisThread.getAnalysisFile(fileList.get(i));
						if (analysisFile.exists()) {
							analysisFile.delete();
							fileList.get(i).analysed=false;
							updateFile(i);
						}   
					}
				}
				return false;
			} 
        });
        uiUpdater = new UiUpdater();
        WindowManager w = getWindowManager();
        Display d = w.getDefaultDisplay();
        width = d.getWidth();
        height = d.getHeight(); 
        Log.d(TAG,width+"x"+height);
        
        // testing code
        Button ff = (Button)findViewById(R.id.ff);
        ff.setOnLongClickListener(new OnLongClickListener() {
			@Override
			public boolean onLongClick(View v) {
				testBuf();
				return true;
			}
        });
        ff.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (synthesisThread!=null && !synthesisThread.isMixing) {
					synthesisThread.ffToMix();
				} else {
					Toast.makeText(AutoMixActivity.this, "Already at mix point", 500).show();
				}
			}
        });
    }
    
    public void updateFile(final int idx) {
    	runOnUiThread(new Runnable() {

			@Override
			public void run() {
				SongDataBean song = fileList.get(idx);
				if (song.analysed) {
					song.view.setBackgroundResource(R.color.analysed); 
					((TextView)song.view.findViewById(R.id.file_tempo)).setText(song.tempo +" ("+song.tempoAvg+") "+((int)(song.matchedBeats/song.numBeats*100))+"%" +" b:"+(int)song.numBeats+" o:"+(int)song.numOnsets);
		        	//((TextView)song.view.findViewById(R.id.file_length)).setText("Time:"+AutoMixActivity.getField(song.nativeSongHandle, SongData.SONG_SECS));
					FrameLayout fr = (FrameLayout)song.view.findViewById(R.id.file_analysed_prg);
					fr.getLayoutParams().width=width;
					song.view.requestLayout();
					
				} else {
					song.view.setBackgroundResource(R.color.analysing);
					if (analysisThread!=null && analysisThread.currentSongIndex!=idx) {
						FrameLayout fr = (FrameLayout)song.view.findViewById(R.id.file_analysed_prg);
						fr.getLayoutParams().width=5;
						fr.getLayoutParams().width=width;
						song.view.requestLayout();
					}
				}
			}
		});
    }
    
    @Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		startTimer();
	}

	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		cancelTimer() ; 
	}
	private void startTimer() {
		if (uiTimer==null) {
			uiTimer = new Timer();
			uiTimer.scheduleAtFixedRate( new UpdateTask(), 0, 2000 );
		}
	}
	private void cancelTimer() {
		if (uiTimer!=null) {
			uiTimer.cancel();
			uiTimer=null;
		}
	}
    
    class UiUpdater implements Runnable{
		@Override
		public void run() { 
			if (analysisThread!=null && analysisThread.currentSongIndex>-1) {
				//Log.d(TAG, "csi:"+analysisThread.currentSongIndex);
				SongDataBean song = fileList.get(analysisThread.currentSongIndex);
				float read = getField(song.nativeSongHandle, SongDataBean.SONG_READ);
				float fs = getField(song.nativeSongHandle, SongDataBean.SONG_SR);
				float prg = width*(read/fs*1000)/(float)song.dur; 
				//Log.d(TAG,read+":"+fs+":"+song.dur+":"+prg);
				FrameLayout fr = (FrameLayout)song.view.findViewById(R.id.file_analysed_prg);
				fr.getLayoutParams().width=(int)prg;
				song.view.requestLayout();
			}
			thisSongDisp.updateDisp();
			nextSongDisp.updateDisp();
		}
    }
	
	private class UpdateTask extends TimerTask{
		@Override
		public void run() {
			runOnUiThread(uiUpdater);
		}
	};
	
	

	public void updateSongDisplay(final SongDispElements el) {
		runOnUiThread(new Runnable () {
			@Override
			public void run() {
				el.updateDisp();
				
			}
		});
		
	}
	
	
	@Override
	protected Dialog onCreateDialog(int id) {
		switch (id) {
			case 1:return new AlertDialog.Builder(this)
		        .setTitle("Song Options")
		        .setMessage("Select an action on this song ...")
		        .setPositiveButton("Re-Analyse", new DialogInterface.OnClickListener() {
		            public void onClick(DialogInterface dialog, int whichButton) {
		            	SongDataBean song = fileList.get(optIndex);
						File analysisFile = AnalysisThread.getAnalysisFile(song);
		            	if (analysisFile.exists()) {analysisFile.delete(); }
		            	song.analysed=false;
		            	updateFile(optIndex);  
		            }
		        })
		        .setNegativeButton("Set Next", new DialogInterface.OnClickListener() {
		            public void onClick(DialogInterface dialog, int whichButton) {
		            	if (synthesisThread!=null && !synthesisThread.isMixing) {
		            		 synthesisThread.queueNextSong(optIndex);
		            	} else {
		            		Toast.makeText(AutoMixActivity.this, "Wait till this mix has finished", 500).show();
		            	} 
		            } 
		        })
		        .setNeutralButton("Play File", new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int whichButton) {
		            	if (synthesisThread==null) {
		            		 startIndex=optIndex;
		            	} else {
		            		Toast.makeText(AutoMixActivity.this, "Already playing...", 500).show();
		            	}
		            }
		        })
		       .create();
		}
		return super.onCreateDialog(id);
	}

	class SongDispElements {
		SongDataBean song;
		TextView fileTxt;
		TextView tempoTxt;
		TextView timeTxt;
		TextView volTxt;
		TextView scaleTxt;
		FrameLayout prg;
		SeekBar volSldr;
		
		public SongDispElements(int fileId,int tmpoId,int timeId,int volId,int prgId,int volSldrId,int scaleId) {
			fileTxt=(TextView)findViewById(fileId);
			tempoTxt=(TextView)findViewById(tmpoId);
			timeTxt=(TextView)findViewById(timeId);
			volTxt=(TextView)findViewById(volId);
			if (scaleId>-1) {scaleTxt=(TextView)findViewById(scaleId);}
			prg=(FrameLayout)findViewById(prgId);
			volSldr=(SeekBar)findViewById(volSldrId);
			volSldr.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
				@Override
				public void onProgressChanged(SeekBar seekBar, int progress,boolean fromUser) {
					song.vol=progress/(float)seekBar.getMax();
				}
				@Override
				public void onStartTrackingTouch(SeekBar seekBar) {
					
				}
				@Override
				public void onStopTrackingTouch(SeekBar seekBar) {
					
				}
			});
			
		}
		public void updateDisp() {
			if (this.song!=null) { 
				String ind = "n";
				switch (song.playState) {
					case SongDataBean.PLAYSTATE_PLAYING: ind = "(p)";	break;
					case SongDataBean.PLAYSTATE_PLAYING_SCALED: ind = "(s)";	break;
					case SongDataBean.PLAYSTATE_QUEUED: ind = "(q)";	break;
					case SongDataBean.PLAYSTATE_STOPPED: ind = "(x)";	break;
					default:break;
				}
				fileTxt.setText(song.disp+ind);
				tempoTxt.setText(song.tempoAvg+" BPM");
				timeTxt.setText((int)(song.playPosition/song.sampleRate)+"/"+song.dur/1000+" sec");
				volTxt.setText(""+(int)(song.vol*200));
				if (scaleTxt!=null) {scaleTxt.setText("scale:"+((int)(song.scaleFactor*100))+"%");}
				
				float prgf = (song.playPosition/song.sampleRate*1000)/(float)song.dur; 
				prg.getLayoutParams().width=(int)(width/2*prgf);
				prg.requestLayout();
			} else {
				fileTxt.setText("-none-");
				tempoTxt.setText("-- BPM");
				timeTxt.setText("-/- sec");
				volTxt.setText("--");
				prg.getLayoutParams().width=1;
				prg.requestLayout();
			}
		}
		public void setSong(SongDataBean song) {
			this.song=song;
			volSldr.setProgress((int)(song.vol*100));
		}
	}
	public static native float getField( int handle,int field );
	/////////////////////////////// testing only ///////////////////////////////////////////////////////////////////////////////////////////
	public void testBuf() {
		
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect( 8192 * Short.SIZE / 8 );
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer theBuffer = byteBuffer.asShortBuffer();
		ByteBuffer byteBuffer1 = ByteBuffer.allocateDirect( 8192 * Short.SIZE / 8 );
        byteBuffer1.order(ByteOrder.nativeOrder());
        ShortBuffer theBuffer1 = byteBuffer.asShortBuffer();
        SynthesisThread.setupPlay(8192);
        for (int j=0;j<4;j++) {
	        int num = test( theBuffer,theBuffer1);
	        Log.d("automixtest",""+num+":"+theBuffer.capacity());
	        theBuffer.rewind();
	        Log.d("automixtest", "------------------------------------------------out-"+j+"------------------------------------------------------------");
	        print(theBuffer);
	        Log.d("automixtest", "------------------------------------------------in-"+j+"------------------------------------------------------------");
	        print(theBuffer1);
	        Log.d("automixtest", "------------------------------------------------end------------------------------------------------------------");
        }
        SynthesisThread.tearDownPlay();
       
	}

	private void print(ShortBuffer theBuffer) {
		StringBuffer s = new StringBuffer();
		for (int i=0;i<theBuffer.capacity();i++) {
			s.append(theBuffer.get(i));
			s.append(":");
			if (i%512==0) {
				 Log.d("automixtest", s.toString());
				 s=new StringBuffer();
			}
		}
	}
	
	public static native int test( ShortBuffer buffer,ShortBuffer buffer1);
}