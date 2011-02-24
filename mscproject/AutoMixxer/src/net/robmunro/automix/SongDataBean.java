package net.robmunro.automix;
/** ********************************************************************************
 * SongDataBean
 * @author robm
 *
 */
import java.io.File;

import android.widget.FrameLayout;

public class SongDataBean {
	
	public static final int SONG_PROGRESS = 1;
	public static final int SONG_TEMPO = 2;
	public static final int SONG_AVGTEMPO = 3;
	public static final int SONG_SR = 4;
	public static final int SONG_SECS = 5;
	public static final int SONG_READ = 6;
	public static final int SONG_PEAKVOL = 7;
	public static final int SONG_AVGVOL = 8;
	public static final int SONG_MATCHEDBEATS = 9;
	public static final int SONG_NUMBEATS = 10;
	public static final int SONG_NUMONSETS = 11;
	
	public static final int PLAYSTATE_NONE = -1;
	public static final int PLAYSTATE_STOPPED = 0;
	public static final int PLAYSTATE_QUEUED = 1;
	public static final int PLAYSTATE_PLAYING = 2;
	public static final int PLAYSTATE_PLAYING_SCALED = 3;
	
	
	//file info
	public String disp;
	public String path;
	public File file;
	public FrameLayout view;
	
	// native handles
	public int nativeSongHandle;
	public int nativeMP3Handle;
	
	//play data
	public int playState=PLAYSTATE_NONE;
	public long playPosition = 0;
	public float scaleFactor = 1;
	public long dur=-1;
	public float sampleRate=44100;
	public float bitRate=44100;
	public float vol=1;
	
	// analysis
	public boolean analysed=false;
	public float tempo = -1;
	public float tempoAvg = -1;
	public float peakVol = -1;
	public float avgVol = -1;
	public float numBeats = -1;
	public float numOnsets = -1;
	public float matchedBeats = 1;
	public int getRemaining() {
		int remaining = (int)(dur/1000 - playPosition/sampleRate);
		return remaining;
	}
}
