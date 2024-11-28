void simboloSetaDireita() {
  VFD.write(0x19); // Fonte Katakana
  VFD.write(0x99); // Simbolo >
  VFD.write(0x18); // Fonte Internacional

}


void salvandoAnimacao() { // Exibe na Tela a animação Salvando...
  moveCursor(0);
  VFD.print("Salvando");
  delay(300);
  moveCursor(8);
  VFD.print(".");
  delay(300);
  moveCursor(9);
  VFD.print(".");
  delay(300);
  moveCursor(10);
  VFD.print(".");

}


void resetVFD() { // Efetua o Reset via software da controladora VFD

  VFD.write(0x1B); // ESC
  VFD.write(0x49); // software reset

}


void VFDclearsceen() { // moves cursor to top-left and clears display

  VFD.write(0x0E); // clear display
  VFD.write(0x0C); // form feed - cursor to top-left

}


void VFDbrightness(int amount) { // Seta Brilho do Display VFD - 25/50/75/100%

  switch (amount) {

    case 25:
      VFD.write(0x1B); // ESC
      VFD.write(0x4C); // brightness
      VFD.print(0); // 25% brightness
      VFDclearsceen();
      moveCursor(0);
      VFD.print("Brilho 25%");
      delay(500);
      VFDclearsceen();
      brilhoAtual = 25;
      break;


    case 50:
      VFD.write(0x1B); // ESC
      VFD.write(0x4C); // brightness
      VFD.write(0x40); // 50% brightness
      VFDclearsceen();
      moveCursor(0);
      VFD.print("Brilho 50%");
      delay(500);
      VFDclearsceen();
      brilhoAtual = 50;
      break;


    case 75:
      VFD.write(0x1B); // ESC
      VFD.write(0x4C); // brightness
      VFD.write(0x80); // 75% brightness
      VFDclearsceen();
      moveCursor(0);
      VFD.print("Brilho 75%");
      delay(500);
      VFDclearsceen();
      brilhoAtual = 75;
      break;


    case 100:
      VFD.write(0x1B); // ESC
      VFD.write(0x4C); // brightness
      VFD.write(0xC0); // 100% brightness
      VFDclearsceen();
      moveCursor(0);
      VFD.print("Brilho 100%");
      delay(500);
      VFDclearsceen();
      brilhoAtual = 100;
  }
}


void VFDchars() { // run through characters for selected font

  for (int i = 21 ; i < 256; i++) {

    VFD.write(0x16); // underline cursor off
    VFD.write(i);
    delay(100);
  }
}

void moveCursor(byte position) {   // moves the cursor - top row is 0~39, bottom row is 40~79  // vertical scroll mode must be turned off if used

  VFD.write(0x1B); // ESC
  VFD.write(0x48); // move cursor
  VFD.write(position); // location

}
