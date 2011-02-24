package net.robmunro.mscproj.tt;
/** ********************************************************************************
 * TAPTEMPO
 * @author robm
 *
 */
import java.math.BigDecimal;
import java.util.Vector;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.SeekBar.OnSeekBarChangeListener;

public class TapTempoActivity extends Activity {
    /** Called when the activity is first created. */
	Vector<Long> tapData = new Vector<Long>();
	long lastTime = 0;
	TextView tempoText =  null;
	TextView instTempoText =  null;
	TextView instTempoLenText =  null;
	Button tapBut =  null;
	Button resetBut =  null;
	SeekBar instTempoLenSeek = null;
	int instTempoLen=10;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        tempoText = (TextView) findViewById(R.id.tt_tempo_text);
        instTempoText = (TextView) findViewById(R.id.tt_tempo_inst_text);
        instTempoLenText = (TextView) findViewById(R.id.tt_inst_len_txt);
        instTempoLenSeek = (SeekBar) findViewById(R.id.tt_inst_len);
        tapBut = (Button) findViewById(R.id.tt_tap_but);
        resetBut = (Button) findViewById(R.id.tt_reset_but);
        tapBut.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				if (lastTime>0) {
					long thisTime = System.currentTimeMillis();
					long beatperiod = thisTime-lastTime;
					if (beatperiod<2000) {
						tapData.add(beatperiod);
						long avgTempo=0;
						for (Long l:tapData) {
							avgTempo+=l;
						}
						setTempo(avgTempo/(float)tapData.size(),tempoText);
						avgTempo=0;
						for (int i=tapData.size()-1;i>=0 && i>=tapData.size()-instTempoLen;i--) {
							avgTempo+=tapData.get(i);
						}
						setTempo(avgTempo/(float)instTempoLen,instTempoText);
					}
				}
				lastTime=System.currentTimeMillis();
			}
        });
        
        resetBut.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View v) {
				lastTime=0;
				tapData.clear();
				setTempo(0,tempoText);
				setTempo(0,instTempoText);
			}
        });
        instTempoLenSeek.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
				instTempoLen=progress;
				instTempoLenText.setText(""+instTempoLen);
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				
			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar) {
				// TODO Auto-generated method stub
				
			}
        	
        });
        instTempoLenSeek.setProgress(instTempoLen);
    }
    
    private void setTempo(float l, TextView tempoText) {
    	float bpm = 60*1000/l;
    	if (l==0) {bpm=0;}
    	BigDecimal d = new BigDecimal(bpm).setScale(2, BigDecimal.ROUND_HALF_EVEN);
		tempoText.setText(d.toString()+" BPM");
		
	}
}