void ldrLeitura() {

  LDRValue = map(analogRead(ldr_pin), 0, 600, 0, 60); // Remapeia um número de um intervalo para outro.

  if (LDRValue > 2 && brilhoAtual != 100) { // Brilho VFD 100%

    VFDbrightness(100);
    antTemp = -1;
    antMinutos = -1;
  }


  if (LDRValue < 2 && brilhoAtual != 50) { // Brilho VFD 50%

    VFDbrightness(50);
    antTemp = -1;
    antMinutos = -1;
  }

}

void ldrSerial() { // Função exibe no Terminal Serial o valor atual do LDR

  Serial.print("Valor lido pelo LDR = ");//Imprime na serial a mensagem Valor lido pelo LDR
  Serial.println(LDRValue);//Imprime na serial os dados de valorldr
  delay(500);


}
