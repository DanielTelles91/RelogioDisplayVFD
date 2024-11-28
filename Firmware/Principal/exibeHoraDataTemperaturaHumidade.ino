void exibeHoraDataTemperaturaHumidade() {

  t = rtc.getTime();

  if (t.sec >= 0 && t.sec <= 30) { //Entre 0 - 30 Segundos, exibe Hora e Data

    if (antMinutos != t.min) {  // Evita enviar dados repetidos na serial desnecessário. "controle remoto para de funcionar caso ao contrário"

      antMinutos = t.min;         // Armazena na varável antMinutos o minutos atual do relógio

      if (alarmeProgramado == true) {
        moveCursor(0);                // Posição do Elemento no Display
        VFD.write(0x19);              // Fonte Katakana
        VFD.write(0x96);              // Simbolo Relógio
      }


      moveCursor(1);                // Posição do Elemento no Display
      VFD.write(0x18);              // Fonte International
      VFD.print(rtc.getTimeStr());  // Exibe o Horário

      moveCursor(8);                // Posição do Elemento no Display
      VFD.print(rtc.getDOWStr());   // Exibe dia da semana

      moveCursor(12);               // Posição do Elemento no Display
      VFD.print(rtc.getDateStr());  // Exibe a Data

    }


    if (t.sec == 30) {              //Quando chegar nos 30 Segundos, faz o efeito de Rolagem Horizontal

      for (int i = 1; i < 20; i++) {

        moveCursor(i);
        VFD.write(0x18);            // Fonte International
        VFD.print(rtc.getTimeStr());
        VFD.print("   ");
        moveCursor(8 + i);
        VFD.print(rtc.getDOWStr()); // Exibe dia da semana
        VFD.print("  ");
        moveCursor(12 + i);
        VFD.print(rtc.getDateStr());
        moveCursor(i - 1);
        VFD.print(" ");

        moveCursor(19);
        VFD.print(" ");

        delay(50);
      }
      VFDclearsceen(); // Após o termino do efeito, limpa a tela
      antTemp = -1;
      antHumidade = -1;
      antMinutos = -1;
    }



  }


  else { // Entre 30 e 60 Segundos, exibe Temperatura e Humidade


    DHT11.read(DHT11_PIN);
    temp = DHT11.temperature - 2;
    humidade = DHT11.humidity;


    if (antTemp != temp || antHumidade != humidade) {

      antTemp = temp;
      antHumidade = humidade;

      moveCursor(0);
      VFD.print("Temp:");
      VFD.print(temp);
      VFD.write(0xB0); // Simbolo º Graus
      VFD.print("C");

      VFD.print("    Hum:");
      VFD.print(humidade);
      VFD.print("%");


    }



    if (t.sec == 59) { // 5 Segundos antes de sair TEMP e Humidade da tela, faz o efeito de rolagem hoizontal

      for (int i = 0; i < 20; i++) {

        VFD.write(0x16); // underline cursor off
        moveCursor(i);
        VFD.print("Temp:");
        VFD.print(temp);
        VFD.write(0xB0); // Simbolo º Graus
        VFD.print("C");
        // VFD.print("");

        VFD.print("    Hum:");
        VFD.print(humidade);
        VFD.print("%");
        moveCursor(i - 1);
        VFD.print(" ");

        moveCursor(19);
        VFD.print(" ");
        delay(50);

      }
      VFDclearsceen();
      antTemp = -1;
      antHumidade = -1;
      antMinutos = -1;
    }

  }

}
