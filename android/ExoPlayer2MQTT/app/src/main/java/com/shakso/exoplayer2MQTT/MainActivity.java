package com.shakso.exoplayer2MQTT;

import android.net.Uri;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import android.util.Log;
import android.view.Surface;

import com.shakso.exoplayer2MQTT.helpers.MQTTHelper;
import com.google.android.exoplayer2.ExoPlaybackException;
import com.google.android.exoplayer2.ExoPlayer;
import com.google.android.exoplayer2.ExoPlayerFactory;
import com.google.android.exoplayer2.Format;
import com.google.android.exoplayer2.PlaybackParameters;
import com.google.android.exoplayer2.SimpleExoPlayer;
import com.google.android.exoplayer2.Timeline;
import com.google.android.exoplayer2.decoder.DecoderCounters;
import com.google.android.exoplayer2.source.LoopingMediaSource;
import com.google.android.exoplayer2.source.MediaSource;
import com.google.android.exoplayer2.source.TrackGroupArray;
import com.google.android.exoplayer2.source.hls.HlsMediaSource;
import com.google.android.exoplayer2.trackselection.AdaptiveTrackSelection;
import com.google.android.exoplayer2.trackselection.DefaultTrackSelector;
import com.google.android.exoplayer2.trackselection.TrackSelection;
import com.google.android.exoplayer2.trackselection.TrackSelectionArray;
import com.google.android.exoplayer2.trackselection.TrackSelector;
import com.google.android.exoplayer2.ui.PlayerView;
import com.google.android.exoplayer2.ui.SimpleExoPlayerView;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DefaultBandwidthMeter;
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;
import com.google.android.exoplayer2.util.Util;
import com.google.android.exoplayer2.video.VideoRendererEventListener;

import org.eclipse.paho.client.mqttv3.*;

public class MainActivity extends AppCompatActivity implements VideoRendererEventListener {


    private static final String TAG = "MainActivity";
    private PlayerView simpleExoPlayerView;
    private SimpleExoPlayer player;
    private MediaSource videoSource;
    MQTTHelper mqttHelper;
    int pos = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        startMqtt();

        Uri mp4VideoUri = Uri.parse("http://192.168.1.2:8008/stream.m3u8");

        DefaultBandwidthMeter bandwidthMeter = new DefaultBandwidthMeter();

        TrackSelection.Factory videoTrackSelectionFactory = new AdaptiveTrackSelection.Factory(bandwidthMeter);
        TrackSelector trackSelector =
                new DefaultTrackSelector(videoTrackSelectionFactory);

        player = ExoPlayerFactory.newSimpleInstance(this, trackSelector);
        simpleExoPlayerView = new SimpleExoPlayerView(this);
        simpleExoPlayerView = (SimpleExoPlayerView) findViewById(R.id.player_view);
        simpleExoPlayerView.setUseController(false);//set to true or false to see controllers
        simpleExoPlayerView.requestFocus();
        simpleExoPlayerView.setPlayer(player);

        DataSource.Factory dataSourceFactory = new DefaultDataSourceFactory(this, Util.getUserAgent(this, "exoplayer2example"), bandwidthMeter);

        MediaSource videoSource = new HlsMediaSource(mp4VideoUri, dataSourceFactory, 1, null, null);
        final LoopingMediaSource loopingSource = new LoopingMediaSource(videoSource);
        player.prepare(videoSource);

        player.addListener(new ExoPlayer.EventListener() {

            @Override
            public void onTimelineChanged(Timeline timeline, Object manifest, int reason) {

            }

            @Override
            public void onTracksChanged(TrackGroupArray trackGroups, TrackSelectionArray trackSelections) {

            }

            @Override
            public void onLoadingChanged(boolean isLoading) {

            }

            @Override
            public void onPlayerStateChanged(boolean playWhenReady, int playbackState) {
                if (playbackState == 3) {
                    String msg = "OK";
                    MqttMessage m = new MqttMessage();
                    m.setPayload(msg.getBytes());
                    try {
                        mqttHelper.mqttAndroidClient.publish("videowindow", m);
                    } catch (MqttException e) {
                        e.printStackTrace();
                    }
                }
            }

            @Override
            public void onRepeatModeChanged(int repeatMode) {

            }

            @Override
            public void onShuffleModeEnabledChanged(boolean shuffleModeEnabled) {

            }

            @Override
            public void onPlayerError(ExoPlaybackException error) {
                Log.v(TAG, "Listener-onPlayerError...");
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                player.stop();
                player.prepare(loopingSource);
                player.setPlayWhenReady(true);
            }

            @Override
            public void onPositionDiscontinuity(int reason) {

            }

            @Override
            public void onPlaybackParametersChanged(PlaybackParameters playbackParameters) {

            }

            @Override
            public void onSeekProcessed() {

            }
        });
        player.setPlayWhenReady(true);
    }


    @Override
    public void onVideoEnabled(DecoderCounters counters) {

    }

    @Override
    public void onVideoDecoderInitialized(String decoderName, long initializedTimestampMs, long initializationDurationMs) {

    }

    @Override
    public void onVideoInputFormatChanged(Format format) {

    }

    @Override
    public void onDroppedFrames(int count, long elapsedMs) {

    }

    @Override
    public void onVideoSizeChanged(int width, int height, int unappliedRotationDegrees, float pixelWidthHeightRatio) {

    }

    @Override
    public void onRenderedFirstFrame(Surface surface) {

    }

    @Override
    public void onVideoDisabled(DecoderCounters counters) {

    }
//-------------------------------------------------------ANDROID LIFECYCLE---------------------------------------------------------------------------------------------

    @Override
    protected void onStop() {
        super.onStop();
        Log.v(TAG, "onStop()...");
    }

    @Override
    protected void onStart() {
        super.onStart();
        Log.v(TAG, "onStart()...");
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.v(TAG, "onResume()...");
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.v(TAG, "onPause()...");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.v(TAG, "onDestroy()...");
        player.release();
    }

    private void startMqtt() {
        mqttHelper = new MQTTHelper(getApplicationContext());
        mqttHelper.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {

            }

            @Override
            public void connectionLost(Throwable throwable) {

            }

            @Override
            public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {
                if ((mqttMessage.toString().contains("LOC="))) {
                    player.stop();
                    Thread.sleep(1000);
                    player.prepare(videoSource);
                    player.setPlayWhenReady(true);
                } else {
                    int newPos = Integer.parseInt(mqttMessage.toString().replace("POS=", "")) - 100;
                    simpleExoPlayerView.animate().setDuration(100).translationY(newPos);
                }
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {

            }
        });
    }
}