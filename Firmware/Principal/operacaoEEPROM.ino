

bool leituraEepromAlarme() { // Faz a Leitura da Memória EEPROM se o Alarme está Programado para Tocar

  bool estado;
  estado = EEPROM.read(2); // Posição 2 da Memória EEPROM
  return estado;

}




void EscritaEepromAlarme(bool estado) { // Faz a Escrita da Memória EEPROM, Alarme Programado ou nao.

  EEPROM.update(2, estado); // Posição 2 da Memória EEPROM


}



int leituraEepromCampainha() { // Faz a Leitura da Memória EEPROM de qual Campainha
  int estadoCampainha;
  estadoCampainha = EEPROM.read(3); // Posição 3 da Memória EEPROM
  return estadoCampainha;

}



void EscritaEepromCampainha(int estadoCampainha) { // Faz a Escrita da Memória EEPROM, qual Campainha Selecionada

  EEPROM.update(3, estadoCampainha); // Posição 3 da Memória EEPROM


}







void leituraEEPROM() {

  alarmeHoras = EEPROM.read(0);
  alarmeMinutos = EEPROM.read(1);

}

void gravacaoEEPROM() {

  EEPROM.update(0, alarmeHoras);
  EEPROM.update(1, alarmeMinutos);

}
