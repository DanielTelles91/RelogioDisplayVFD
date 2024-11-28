
void somInicializacao() {

  tone(buzzer, 620);   // Liga o buzzer com uma frequência de 700 Hz.
  delay(200);          // Intervalo de 100 milissegundos.
  noTone(buzzer);     // Desliga o buzzer.

}


void somTeclado() {

  tone(buzzer, 6300); // Liga o buzzer com uma frequência de 5000 Hz.
  delay(50);         // Intervalo de 100 milissegundos.
  noTone(buzzer);     // Desliga o buzzer.

}
