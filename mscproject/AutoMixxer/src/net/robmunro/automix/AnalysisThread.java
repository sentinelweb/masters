package net.robmunro.automix;
/** ********************************************************************************
 * AnalysisThread extends Thread
 * @author robm
 *  
 */
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Vector;

import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

public class AnalysisThread extends Thread {
	Vector<SongDataBean> fileList = null;
	AutoMixActivity ama = null;
	public int currentSongIndex = -1;
	public boolean finished = false;
	public boolean finishAfterNext = false;
	
	static File summaryFile;
	String summaryFilePath="/sdcard/test/summary.txt";
	
	public AnalysisThread(AutoMixActivity ama,Vector<SongDataBean> fileList) {
		super();
		this.fileList = fileList;
		this.ama=ama;
		summaryFile=new File(summaryFilePath);
        
	}
	
	
	public void run() {
		for  (int i=0;i<this.fileList.size();i++) {
			SongDataBean song = this.fileList.get(i);
			if (!song.analysed) {
				if (getAnalysisFile(song).exists()) {
					loadAnalysisFile(song);
					song.analysed=true;   
				} else {
					currentSongIndex=i;
					//Log.d(AutoMixActivity.TAG, "allocate");
					int songHandle = allocateSong(  ); 
					song.nativeSongHandle = songHandle;     
					long start = System.currentTimeMillis();
					Log.d(AutoMixActivity.TAG, "analyse"); 
					ama.updateFile(i);
					analyseSong( song.nativeSongHandle,song.path );
					Log.d(AutoMixActivity.TAG, "finished:"+(System.currentTimeMillis()-start));
					song.analysed=true;  
					song.tempo = AutoMixActivity.getField(song.nativeSongHandle, SongDataBean.SONG_TEMPO);
					song.tempoAvg = AutoMixActivity.getField(song.nativeSongHandle, SongDataBean.SONG_AVGTEMPO);
					song.peakVol = AutoMixActivity.getField(song.nativeSongHandle, SongDataBean.SONG_PEAKVOL);
					song.avgVol = AutoMixActivity.getField(song.nativeSongHandle, SongDataBean.SONG_AVGVOL);
					song.matchedBeats = AutoMixActivity.getField(song.nativeSongHandle, SongDataBean.SONG_MATCHEDBEATS);
					song.numBeats = AutoMixActivity.getField(song.nativeSongHandle, SongDataBean.SONG_NUMBEATS);
					song.numOnsets = AutoMixActivity.getField(song.nativeSongHandle, SongDataBean.SONG_NUMONSETS); 
					
					saveAnalysisFile(song);
					
					if (summaryFile!=null) {
						try {
							FileWriter summaryFileWriter = new FileWriter(summaryFile,true);
							summaryFileWriter.write(song.path+","+song.tempo+","+song.tempoAvg+","+song.numBeats+","+(song.matchedBeats/song.numBeats*100)+","+song.numOnsets+"\n");
							summaryFileWriter.flush();
							summaryFileWriter.close();
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}
				
			}
			ama.updateFile(i);
			if (finishAfterNext) {	break;	}
		}
		currentSongIndex=-1;
		finished=true;
		ama.runOnUiThread(new Runnable() {public void run () {Toast.makeText(ama, "Analysis finished", 500).show();}});
		
	}
	private void loadAnalysisFile(SongDataBean song) {
		File f = getAnalysisFile(song);
		try {
			BufferedReader fr = new BufferedReader(new FileReader(f));
			String line = null;
			while ((line = fr.readLine())!=null) {
				String[] split = line.split(":");
				float val = Float.parseFloat(split[1]);
				if (split[0].equals("T")) {
					song.tempo=val;
				} else if (split[0].equals("AT")){
					song.tempoAvg=val;
				} else if (split[0].equals("PV")){
					song.peakVol=val;
				} else if (split[0].equals("AV")){
					song.avgVol=val;
				} else if (split[0].equals("MB")){
					song.matchedBeats=val;
				} else if (split[0].equals("NB")){
					song.numBeats=val;
				} else if (split[0].equals("NO")){
					song.numOnsets=val;
				}
			}
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}


	private void saveAnalysisFile(SongDataBean song) {
		File f = getAnalysisFile(song);
		try {
			FileWriter fw = new FileWriter(f);
			fw.write("T:"+song.tempo+"\n");
			fw.write("AT:"+song.tempoAvg+"\n");
			fw.write("PV:"+song.peakVol+"\n");
			fw.write("AV:"+song.avgVol+"\n");
			fw.write("MB:"+song.matchedBeats+"\n");
			fw.write("NB:"+song.numBeats+"\n");
			fw.write("NO:"+song.numOnsets+"\n");
			fw.close(); 
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace(); 
		}
	}


	public static File getAnalysisFile(SongDataBean song) {
		String fileName = song.path.substring(0,song.path.lastIndexOf("."))+".ana";
		File f = new File(fileName);
		return f;
	}


	public native int allocateSong(  );
	public native int analyseSong( int handle,String fileName );
}
