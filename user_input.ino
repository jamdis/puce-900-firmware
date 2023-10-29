void initUserInput(){
  pinMode( ME_SWITCH, INPUT_PULLUP );
  pinMode( PLAYBACK_SWITCH, INPUT_PULLUP);
  pinMode(SHUTTER_BUTTON, INPUT_PULLUP );
  pinMode( VIDEO_MODE_SWITCH, INPUT_PULLUP);
  //registerRoataryInterrupts();
}

void registerRoataryInterrupts(){
  pinMode( ROTARY_ENCODER_L, INPUT_PULLUP );
  pinMode( ROTARY_ENCODER_R, INPUT_PULLUP );

  attachInterrupt( digitalPinToInterrupt( ROTARY_ENCODER_L ), rotaryL, FALLING);
  attachInterrupt( digitalPinToInterrupt( ROTARY_ENCODER_R ), rotaryR, HIGH);

}

void rotaryL(){
  Serial.println("L");
}

void rotaryR(){
  Serial.println("R");
}


void updateUserInput(){
  ui.me_switch = digitalRead( ME_SWITCH ); //what does the ME switch say?
  ui.playback_switch =  !digitalRead(PLAYBACK_SWITCH); //invert because of labelling on case
  ui.video_mode_switch = digitalRead(VIDEO_MODE_SWITCH);
  ui.shutter_button_down = !digitalRead( SHUTTER_BUTTON ) ; //invert input because pullup.
  ui.shutter_button_pressed = false;
  if( ui.shutter_button_down && millis() > ui.shutter_ready_at){
    ui.shutter_ready_at = millis() + SHUTTER_COOLDOWN;
    ui.shutter_button_pressed = true;
  }
  ui.exposure_pot = analogRead( EXPOSURE_POT );
  ui.exposure_index = map(ui.exposure_pot, 0, 4095, 0, 11);
  ui.exposure_display = exposure_table[ui.exposure_index][0];
  ui.exposure_period = exposure_table[ui.exposure_index][1];


}


bool getVideoModeSwitch(){
  return ui.video_mode_switch;
}

bool getMeSwitch(){
  return ui.me_switch;
}

bool getPlaybackSwitch(){
  return ui.playback_switch;
}


bool getShutterButton(){
  return ui.shutter_button_pressed;
}

uint getExpPot(){
  return ui.exposure_pot;
}