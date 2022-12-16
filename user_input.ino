void initUserInput(){
  pinMode( AE_SWITCH, INPUT_PULLUP );
  pinMode(SHUTTER_BUTTON, INPUT_PULLUP );
}

void updateUserInput(){
  ui.ae_switch = digitalRead( AE_SWITCH );
  ui.shutter_button_down = !digitalRead( SHUTTER_BUTTON ) ; //invert input because pullup.
  ui.shutter_button_pressed = false;
  if( ui.shutter_button_down && millis() > ui.shutter_ready_at){
    ui.shutter_count ++;
    ui.shutter_ready_at = millis() + SHUTTER_COOLDOWN;
    ui.shutter_button_pressed = true;
  }
  ui.exposure_pot = analogRead( EXPOSURE_POT );

}

bool getAeSwitch(){
  return ui.ae_switch;
}

bool getShutterButton(){
  return ui.shutter_button_pressed;
}

uint getExpPot(){
  return ui.exposure_pot;
}