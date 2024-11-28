// Desenvolvido por Daniel Arantes Telles
// 2020-2023


// Drivers
#include <DS3231.h>                     // Biblioteca Módulo RTC
#include <dht11.h>                      // Biblioteca Sensor Úmidade e Temperatura DHT11
#include <IRremote.h>                   // Biblioteca Controle Remoto
#include <SoftwareSerial.h>             // Biblioteca Comunicação Serial do Display VFD
#include "Wire.h"                       // I2C interface 
#include <EEPROM.h>                     // Driver de Comunicação com a EEPROM

// Mapeamento dos Pinos
#define IR_RECEIVE_PIN 9                // Pino D9 - Ligado ao Sensor IR (Controle Remoto)
#define DHT11_PIN 7                     // Pino D7 - Ligado ao Sensor Temperatura e Úmidade 
int ldr_pin = A6;                       // Atribui o Pino A6(Porta Analogica) à variável ldr_pin
int bat_pin = A0;                       // Atribui o Pino A0(Porta Analogica) à variável bat_pin
int buzzer = 12;                        // Atribui o Pino 12(Porta Digital) à variável buzzer
SoftwareSerial VFD(0, 1);               // Pino do Display RX, TX
DS3231  rtc(SDA, SCL);


// Mapeamento do Controle Remoto
#define IR_BUTTON_Brilho 22             // Tecla * - Altera o Brilho do Display VFD
#define IR_BUTTON_Configuracao 13       // Tecla # - Menu 1)Data e Hora e Alarme
#define IR_BUTTON_SetaDireita 90        // Tecla ⇨ - Seta Direcional DIREITA
#define IR_BUTTON_SetaEsquerda 8        // Tecla ⇦ - Seta Direcional ESQUERDA
#define IR_BUTTON_Ok 28                 // Tecla OK
#define IR_BUTTON_SetaCima 24           // Tecla ⇧ - Seta Direcional CIMA
#define IR_BUTTON_Baixo 82              // Tecla ⇩ - Seta Direcional BAIXO
#define IR_BUTTON_1 69                  // Tecla 1 - Botão 1 - Ativa/Desativa o Alarme
#define IR_BUTTON_2 70                  // Tecla 2 - Botão 1 - Ativa/Desativa o Alarme



// Variáveis

Time t;
dht11 DHT11;

bool menuConfig, ConfigDataHora, ConfigHoras, ConfigMinutos, ConfigDia, ConfigMes, ConfigAno, ConfigDiaSemana = false;
bool ConfigAlarme, ConfigHorasAlarme, ConfigMinutosAlarme, ConfigCampainha, alarmeProgramado, desligaAgoraAlarme = false;

int valorBrilho = 100;
int count = 0;
int temp, humidade  = 0;
int antTemp, antHumidade, antMinutos  = -1;
int horas, minutos, segundos, dia, mes, ano = 0;
int sethoras, setminutos, setdia, setmes, setano, setdiasemana = 0;
int alarmeHoras, alarmeMinutos, estadoCampainha = 0;

String setdiasemanaP;

String diadasemana[7] = {"Seg", "Ter", "Qua", "Qui", "Sex", "Sab", "Dom"};
int indicearraydiasemana = 0;



int LDRValue = 0;
int brilhoAtual = 0;
int brilhoTemp = 0;


float input_voltage = 0.0;


// Variáveis do Som Doom e Pantera
int tempo, notes, wholenote, divider, noteDuration = 0;


void setup() {

  Wire.begin();                      // Inicializa I2C Interface
  rtc.begin();                       // Inicializa o módulo do Relógio
  VFD.begin(19200);                  // Seta a velocidade serial entre o Arduino e Display VFD
  resetVFD();                        // Reseta o Display VFD
  VFDclearsceen();                   // Limpa o Display VFD
  VFD.write(0x12);                   // Ativa Vertical Scroll Mode (on)
  IrReceiver.begin(IR_RECEIVE_PIN);  // Ativa o sensor Infra Vermelho do Controle Remoto,
  pinMode(ldr_pin, INPUT);           // Define ldr (pino analógico A6)
  pinMode(bat_pin, INPUT);           // Define bat (pino analógico A0)
  pinMode(buzzer, OUTPUT);           // Definindo o pino buzzer como de saída.
  alarmeProgramado = leituraEepromAlarme();
  estadoCampainha = leituraEepromCampainha();
  leituraEEPROM();
  somInicializacao();
  //Serial.begin(9600);           // Necessário Ativar quando usar Monitor Serial (Display VFD para de funcionar quando ativado essa função)



  // rtc.setDOW(1);     // Set Day-of-Week to SUNDAY
  // rtc.setTime(1, 35, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(22, 3, 2023);   // Set the date to January 1st, 2014


}




void leituraTensaoBateria() {
  int analog_value = analogRead(A0);
  input_voltage = (analog_value * 5.0) / 1024.0;



}












void controleRemoto() {

  if (IrReceiver.decode()) {

    somTeclado();

    IrReceiver.resume();
    int command = IrReceiver.decodedIRData.command;
    //   Serial.println(IrReceiver.decodedIRData.command);

    switch (command) {

      case IR_BUTTON_Brilho: { // BOTÃO *****************************************************************************************************************************************
          desligaAgoraAlarme = true;
          count = 1;
          antTemp = -1;
          antMinutos = -1;

          if (valorBrilho == 25 && count == 1) {

            VFDbrightness(50);
            valorBrilho = 50;
            count = 0;

          }

          if (valorBrilho == 50 && count == 1) {

            VFDbrightness(75);
            valorBrilho = 75;
            count = 0;

          }

          if (valorBrilho == 75 && count == 1) {

            VFDbrightness(100);
            valorBrilho = 100;
            count = 0;

          }

          if (valorBrilho == 100 && count == 1) {

            VFDbrightness(25);
            valorBrilho = 25;
            count = 0;

          }

          break;

        }





      case IR_BUTTON_Configuracao: { // BOTÃO #  Abre ou Fecha o Menu de Configuração

          if (menuConfig == false) { // Se Pressionado o Botão Configuração e o Menu nao estiver aberto, o mesmo é aberto

            menuConfig = true;
            VFDclearsceen();
            moveCursor(4);
            VFD.print("Configuracao");
            delay(1000);
            VFDclearsceen();
            moveCursor(1);
            VFD.print("Data/Hora");
            moveCursor(14);
            VFD.print("Alarme");
            break;
          }

          if (menuConfig == true) { // Se o botao for pressionado novamente e ja estiver no menu, volta para tela de horas e data

            menuConfig = false;
            ConfigDataHora = false;
            ConfigAlarme = false;
            VFDclearsceen();
            antMinutos = 99;
            antTemp = 99;
            break;

          }

        }







      case IR_BUTTON_SetaDireita: { // BOTÃO ⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨⇨

          if (menuConfig == true) { // Se Estiver no Menu de Configuração (Data/Hora - Alarme)

            if (ConfigDataHora == false && ConfigAlarme == false) { // 1º Aperto do Botão ⇨, a seta > é direcionada para Data/Hora

              ConfigDataHora = true;
              ConfigAlarme = false;
              moveCursor(0);
              simboloSetaDireita();
              break;

            }

            if (ConfigDataHora == true && ConfigAlarme == false) { // 2º Aperto do Botão ⇨, a seta > é direcionada para Alarme

              ConfigDataHora = false;
              ConfigAlarme = true;
              moveCursor(0);
              VFD.print(" ");
              moveCursor(13);
              simboloSetaDireita();
              break;

            }

            if (ConfigDataHora == false && ConfigAlarme == true) { // 3º Aperto do Botão ⇨, a seta > é direcioanda para Data/Hora

              ConfigDataHora = true;
              ConfigAlarme = false;
              moveCursor(13);
              VFD.print(" ");
              moveCursor(0);
              simboloSetaDireita();
              break;

            }


          }

        }


      case IR_BUTTON_SetaEsquerda: { // BOTÃO ⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦⇦

          if (menuConfig == true) { // Se Estiver no Menu de Config (Data/Hora - Alarme)

            if (ConfigDataHora == false && ConfigAlarme == false) { // 1º Aperto do Botão ⇦, a seta > é direcionada para Data/Hora

              ConfigDataHora = false;
              ConfigAlarme = true;
              moveCursor(13);
              simboloSetaDireita();
              break;

            }

            if (ConfigDataHora == false && ConfigAlarme == true) { // 2º Aperto do Botão ⇦, a seta > é direcionada para Alarme

              ConfigDataHora = true;
              ConfigAlarme = false;
              moveCursor(13);
              VFD.print(" ");
              moveCursor(0);
              simboloSetaDireita();
              break;

            }

            if (ConfigDataHora == true && ConfigAlarme == false) { // 3º Aperto do Botão ⇦, a seta > é direcioanda para Data/Hora

              ConfigDataHora = false;
              ConfigAlarme = true;
              moveCursor(0);
              VFD.print(" ");
              moveCursor(13);
              simboloSetaDireita();
              break;

            }


          }


        }


      case IR_BUTTON_Ok: { // BOTÃO OKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOKOK

          if (ConfigDataHora == true && ConfigHoras == false && ConfigMinutos == false
              && ConfigDia == false && ConfigMes == false && ConfigAno == false && ConfigDiaSemana == false) { // 1º Exibe a Tela HORA:

            ConfigHoras = true; // Bool ConfigHoras fica como verdadeiro

            t = rtc.getTime();
            sethoras = t.hour;
            setminutos = t.min;
            setdia = t.date;
            setmes = t.mon;
            setano = t.year;   // Pega todos os dados atuais do RTC e armazena nas varáveis correspondentes
            setdiasemanaP = rtc.getDOWStr();


            VFDclearsceen();
            moveCursor(0);
            VFD.print("Horas: ");
            VFD.print(sethoras);  // Nessa momento o TFT exibe a Hora atual e é possivel alterar a hora usando as seta cima/ baixo

            break;
          }


          if (ConfigDataHora == true && ConfigHoras == true && ConfigMinutos == false
              && ConfigDia == false && ConfigMes == false && ConfigAno == false && ConfigDiaSemana == false) { // 2º Exibe a Tela MINUTOS

            ConfigHoras = false;
            ConfigMinutos = true;
            ConfigDia = false;
            ConfigMes = false;
            ConfigAno = false;
            ConfigDiaSemana = false;

            VFDclearsceen();
            moveCursor(0);
            VFD.print("Minutos: ");
            VFD.print(setminutos);

            break;

          }

          if (ConfigDataHora == true && ConfigHoras == false && ConfigMinutos == true
              && ConfigDia == false && ConfigMes == false && ConfigAno == false && ConfigDiaSemana == false) { // 3º Exibe a Tela DIA

            ConfigHoras = false;
            ConfigMinutos = false;
            ConfigDia = true;
            ConfigMes = false;
            ConfigAno = false;
            ConfigDiaSemana = false;

            VFDclearsceen();
            moveCursor(0);
            VFD.print("Dia: ");
            VFD.print(setdia);

            break;

          }


          if (ConfigDataHora == true && ConfigHoras == false && ConfigMinutos == false
              && ConfigDia == true && ConfigMes == false && ConfigAno == false && ConfigDiaSemana == false) { // 4º Exibe a Tela Mês

            ConfigHoras = false;
            ConfigMinutos = false;
            ConfigDia = false;
            ConfigMes = true;
            ConfigAno = false;
            ConfigDiaSemana = false;

            VFDclearsceen();
            moveCursor(0);
            VFD.print("Mes: ");
            VFD.print(setmes);

            break;

          }

          if (ConfigDataHora == true && ConfigHoras == false && ConfigMinutos == false
              && ConfigDia == false && ConfigMes == true && ConfigAno == false && ConfigDiaSemana == false) { // 5º Exibe a Tela ANO

            ConfigHoras = false;
            ConfigMinutos = false;
            ConfigDia = false;
            ConfigMes = false;
            ConfigAno = true;
            ConfigDiaSemana = false;

            VFDclearsceen();
            moveCursor(0);
            VFD.print("Ano: ");
            VFD.print(setano);

            break;

          }

          if (ConfigDataHora == true && ConfigHoras == false && ConfigMinutos == false
              && ConfigDia == false && ConfigMes == false && ConfigAno == true && ConfigDiaSemana == false) { // 6º Exibe a Tela DIA DA SEMANA

            ConfigHoras = false;
            ConfigMinutos = false;
            ConfigDia = false;
            ConfigMes = false;
            ConfigAno = false;
            ConfigDiaSemana = true;

            VFDclearsceen();
            moveCursor(0);
            VFD.print("Dia da Sem: ");
            VFD.print(rtc.getDOWStr());

            break;

          }



          if (ConfigDataHora == true && ConfigHoras == false && ConfigMinutos == false
              && ConfigDia == false && ConfigMes == false && ConfigAno == false && ConfigDiaSemana == true) {

            ConfigHoras = false;
            ConfigMinutos = false;
            ConfigDia = false;
            ConfigMes = false;
            ConfigAno = false;
            ConfigDiaSemana = false;
            menuConfig = false;
            antMinutos = 99;
            antTemp = 99;
            ConfigDataHora = false;
            ConfigAlarme = false;


            VFDclearsceen();
            salvandoAnimacao();
            delay(2000);
            VFDclearsceen();

            break;

          }










          //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

          if (ConfigAlarme == true && ConfigHorasAlarme == false && ConfigMinutosAlarme == false && ConfigCampainha == false) {

            ConfigHorasAlarme = true;
            ConfigMinutosAlarme = false;
            ConfigCampainha = false;
            leituraEEPROM();
            VFDclearsceen();
            moveCursor(0);
            VFD.print("Horas:");
            VFD.print(alarmeHoras);
            break;


          }



          if (ConfigAlarme == true && ConfigHorasAlarme == true && ConfigMinutosAlarme == false && ConfigCampainha == false) {

            ConfigHorasAlarme = false;
            ConfigMinutosAlarme = true;
            ConfigCampainha = false;
            leituraEEPROM();
            VFDclearsceen();
            moveCursor(0);
            VFD.print("Minutos:");
            VFD.print(alarmeMinutos);
            break;


          }

          if (ConfigAlarme == true && ConfigHorasAlarme == false && ConfigMinutosAlarme == true && ConfigCampainha == false) {

            ConfigHorasAlarme = false;
            ConfigMinutosAlarme = false;
            ConfigCampainha = true;
            leituraEepromCampainha();
            VFDclearsceen();
            moveCursor(0);
            VFD.print("Campainha:");
            if (estadoCampainha == 1) {
              VFD.print("Pantera");
            }
            if (estadoCampainha == 2) {
              VFD.print("Doom");
            }

            break;


          }




          if (ConfigAlarme == true && ConfigHorasAlarme == false && ConfigMinutosAlarme == false && ConfigCampainha == true ) {

            ConfigAlarme = false;
            ConfigHorasAlarme = false;
            ConfigMinutosAlarme = false;
            ConfigCampainha = false;
            menuConfig = false;
            antMinutos = 99;
            antTemp = 99;


            VFDclearsceen();
            salvandoAnimacao();
            delay(2000);
            VFDclearsceen();
            break;





          }



          break;

        }


      case IR_BUTTON_SetaCima: { // BOTÃO ⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧⇧

          if (ConfigDataHora == true && ConfigHoras == true) { // Quando estiver na Tela de Ajuste da Hora, a Seta ⇧ fica ativo para Incremento da Hora

            if (sethoras > 22) {

              sethoras = 0;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Horas: ");
              VFD.print(sethoras);
              rtc.setTime(sethoras, 0, 0);

            }

            else {

              sethoras = sethoras + 1;
              moveCursor(0);
              VFD.print("Horas: ");
              VFD.print(sethoras);

              rtc.setTime(sethoras, 0, 0);

            }

            break;
          }


          if (ConfigDataHora == true && ConfigMinutos == true) { // Quando estiver na Tela de Ajuste do Minuto, a Seta ⇧ fica ativo para Incremento do Minuto

            if (setminutos > 58) {

              setminutos = 0;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Minutos: ");
              VFD.print(setminutos);

              rtc.setTime(sethoras, setminutos, 0);

            }

            else {

              setminutos = setminutos + 1;
              moveCursor(0);
              VFD.print("Minutos: ");
              VFD.print(setminutos);

              rtc.setTime(sethoras, setminutos, 0);

            }

            break;
          }


          if (ConfigDataHora == true && ConfigDia == true) { // Quando estiver na Tela de Ajuste do Dia, a Seta ⇧ fica ativo para Incremento do Dia

            if (setdia > 30) {

              setdia = 1;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Dia: ");
              VFD.print(setdia);

              rtc.setDate(setdia, setmes, setano);

            }

            else {

              setdia = setdia + 1;
              moveCursor(0);
              VFD.print("Dia: ");
              VFD.print(setdia);

              rtc.setDate(setdia, setmes, setano);

            }

            break;
          }


          if (ConfigDataHora == true && ConfigMes == true) { // Quando estiver na Tela de Ajuste do Mês, a Seta ⇧ fica ativo para Incremento do Mês

            if (setmes > 11) {

              setmes = 1;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Mes: ");
              VFD.print(setmes);

              rtc.setDate(setdia, setmes, setano);

            }

            else {

              setmes = setmes + 1;
              moveCursor(0);
              VFD.print("Mes: ");
              VFD.print(setmes);

              rtc.setDate(setdia, setmes, setano);
            }

            break;
          }


          if (ConfigDataHora == true && ConfigAno == true) { // Quando estiver na Tela de Ajuste do Ano, a Seta ⇧ fica ativo para Incremento do Ano

            if (setano > 2050) {

              setano = 2000;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Ano: ");
              VFD.print(setano);

              rtc.setDate(setdia, setmes, setano);

            }

            else {

              setano = setano + 1;
              moveCursor(0);
              VFD.print("Ano: ");
              VFD.print(setano);

              rtc.setDate(setdia, setmes, setano);

            }

            break;
          }

          ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

          if (ConfigDataHora == true && ConfigDiaSemana == true) { // Quando estiver na Tela de Ajuste do Ano, a Seta ⇧ fica ativo para Incremento do Ano

            if (indicearraydiasemana == 6) {

              indicearraydiasemana = 0;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Dia da Sem: ");
              VFD.print(diadasemana[indicearraydiasemana]);

              rtc.setDOW(indicearraydiasemana + 1);


            }

            else {

              indicearraydiasemana = indicearraydiasemana + 1;
              moveCursor(0);
              VFD.print("Dia da Sem: ");
              VFD.print(diadasemana[indicearraydiasemana]);

              rtc.setDOW(indicearraydiasemana + 1);

            }

            break;
          }


          /////////////////////////////////////////////////////////////////////// AJUSTE DO ALARME////////////////////////////////////////////////////////////////////////////////////////////////////


          if (ConfigAlarme == true  && ConfigHorasAlarme == true ) { // Quando estiver na Tela de Ajuste Alarme da Hora, a Seta ⇧ fica ativo para Incremento da Hora

            if (alarmeHoras > 22) {

              alarmeHoras = 0;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Horas:");
              VFD.print(alarmeHoras);
              gravacaoEEPROM();

            }

            else {

              alarmeHoras = alarmeHoras + 1;
              moveCursor(0);
              VFD.print("Horas:");
              VFD.print(alarmeHoras);
              gravacaoEEPROM();

            }

            break;
          }




          if (ConfigAlarme == true  && ConfigMinutosAlarme == true ) { // Quando estiver na Tela de Ajuste Alarme da Minutos, a Seta ⇧ fica ativo para Incremento da Hora

            if (alarmeMinutos > 58) {

              alarmeMinutos = 0;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Minutos:");
              VFD.print(alarmeMinutos);
              gravacaoEEPROM();

            }

            else {

              alarmeMinutos = alarmeMinutos + 1;
              moveCursor(0);
              VFD.print("Minutos:");
              VFD.print(alarmeMinutos);
              gravacaoEEPROM();

            }

            break;
          }






























          if (ConfigAlarme == true  && ConfigCampainha == true ) { // Quando estiver na Tela de Ajuste da Campainha, a Seta ⇧ fica ativo para a escolha da campainha

            if (estadoCampainha > 1) {

              estadoCampainha = 1;
              EscritaEepromCampainha(1);
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Campainha:");
              VFD.print("Pantera");


            }

            else {
              estadoCampainha = estadoCampainha + 1;
              EscritaEepromCampainha(estadoCampainha);
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Campainha:");
              VFD.print("Doom");
            }



            break;
          }








































          break;
        }


      case IR_BUTTON_Baixo: { // BOTÃO ⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩⇩

          if (ConfigDataHora == true && ConfigHoras == true) { // Quando estiver na Tela de Ajuste das Horas, a Seta ⇩ fica ativo para Decremento da Hora

            if (sethoras < 1) {

              sethoras = 23;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Horas: ");
              VFD.print(sethoras);

              rtc.setTime(sethoras, 0, 0);

            }

            else {

              sethoras = sethoras - 1;
              moveCursor(0);
              VFD.print("Horas: ");
              VFD.print(sethoras);

              rtc.setTime(sethoras, 0, 0);

            }

            break;

          }



          if (ConfigDataHora == true && ConfigMinutos == true) { // Quando estiver na Tela de Ajuste dos Minutos, a Seta ⇩ fica ativo para Decremento do Minuto

            if (setminutos < 1) {

              setminutos = 59;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Minutos: ");
              VFD.print(setminutos);

              rtc.setTime(sethoras, setminutos, 0);
            }

            else {

              setminutos = setminutos - 1;
              moveCursor(0);
              VFD.print("Minutos: ");
              VFD.print(setminutos);

              rtc.setTime(sethoras, setminutos, 0);
            }

            break;

          }



          if (ConfigDataHora == true && ConfigDia == true) { // Quando estiver na Tela de Ajuste do Dia, a Seta ⇩ fica ativo para Decremento do Dia

            if (setdia < 2) {

              setdia = 30;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Dia: ");
              VFD.print(setdia);

              rtc.setDate(setdia, setmes, setano);

            }

            else {

              setdia = setdia - 1;
              moveCursor(0);
              VFD.print("Dia: ");
              VFD.print(setdia);

              rtc.setDate(setdia, setmes, setano);

            }

            break;

          }


          if (ConfigDataHora == true && ConfigMes == true) { // Quando estiver na Tela de Ajuste do Mês, a Seta ⇩ fica ativo para Decremento do Mês

            if (setmes < 2) {

              setmes = 12;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Mes: ");
              VFD.print(setmes);

              rtc.setDate(setdia, setmes, setano);

            }

            else {

              setmes = setmes - 1;
              moveCursor(0);
              VFD.print("Mes: ");
              VFD.print(setmes);

              rtc.setDate(setdia, setmes, setano);
            }

            break;
          }


          if (ConfigDataHora == true && ConfigAno == true) {  // Quando estiver na Tela de Ajuste do Ano, a Seta ⇩ fica ativo para Decremento do Ano

            if (setano < 2002) {

              setano = 2050;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Ano: ");
              VFD.print(setano);

              rtc.setDate(setdia, setmes, setano);

            }

            else {

              setano = setano - 1;
              moveCursor(0);
              VFD.print("Ano: ");
              VFD.print(setano);

              rtc.setDate(setdia, setmes, setano);

            }

            break;
          }







          /////////////////////////////////////////////////////////////////////// AJUSTE DO ALARME////////////////////////////////////////////////////////////////////////////////////////////////////


          if (ConfigAlarme == true  && ConfigHorasAlarme == true ) { // Quando estiver na Tela de Ajuste Alarme da Hora, a Seta ⇧ fica ativo para Incremento da Hora

            if (alarmeHoras < 1) {

              alarmeHoras = 23;
              moveCursor(0);
              VFD.print("Horas:");
              VFD.print(alarmeHoras);
              gravacaoEEPROM();

            }

            else {

              alarmeHoras = alarmeHoras - 1;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Horas:");
              VFD.print(alarmeHoras);
              gravacaoEEPROM();

            }

            break;
          }




          if (ConfigAlarme == true  && ConfigMinutosAlarme == true ) { // Quando estiver na Tela de Ajuste Alarme da Minutos, a Seta ⇧ fica ativo para Incremento da Hora

            if (alarmeMinutos < 1) {

              alarmeMinutos = 59;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Minutos:");
              VFD.print(alarmeMinutos);
              gravacaoEEPROM();

            }

            else {

              alarmeMinutos = alarmeMinutos - 1;
              VFDclearsceen();
              moveCursor(0);
              VFD.print("Minutos:");
              VFD.print(alarmeMinutos);
              gravacaoEEPROM();

            }

            break;
          }




          break;

        }

      //////////////////////////////////////////////////////////////////////76567657657
      case IR_BUTTON_1: {


          if (t.sec >= 0 && t.sec <= 30) {
            alarmeProgramado = leituraEepromAlarme();

            if (alarmeProgramado == true) {

              EscritaEepromAlarme(false);
              alarmeProgramado = false;
              moveCursor(0);
              VFD.print(" ");
              delay(100);
              break;
            }
            if (alarmeProgramado == false) {

              EscritaEepromAlarme(true);
              alarmeProgramado = true;
              moveCursor(0);
              VFD.write(0x19);              // Fonte Katakana
              VFD.write(0x96);              // Simbolo Relógio
              delay(100);
              break;

            }
            break;



          }

        }



      case IR_BUTTON_2: { // Verifica Estado da Bateria


          leituraTensaoBateria();
          VFDclearsceen();
          moveCursor(0);                // Posição do Elemento no Display
          VFD.write(0x18);              // Fonte International
          VFD.print("Status Bat: ");
          if (input_voltage > 3.20) {
            VFD.print("Bad");
          }
          else {
            VFD.print("Good");
          }
          // VFD.print(input_voltage);  // Exibe o Horário
          delay(5000);
          VFDclearsceen();
          antHumidade = -1;
          antMinutos = -1;
          break;

        }








    }

  }

}






void alarme() {
  if (alarmeProgramado == true && desligaAgoraAlarme == false && alarmeHoras == horas && alarmeMinutos == minutos) {


    brilhoTemp = brilhoAtual;

    if (estadoCampainha == 1) {
      somPanteraRosa();
    }
    if (estadoCampainha == 2) {
      somDoom();
    }


  }

  if (desligaAgoraAlarme == true && alarmeHoras == horas && alarmeMinutos != minutos ) {

    desligaAgoraAlarme = false;
    alarmeProgramado = true;
  }




}





void loop() {

  controleRemoto();

  if (menuConfig != true) {        // Se não estiver na Tela de Configuração(Data/Hora e Alarme), exibe a tela info , data/hora e temp

    ldrLeitura();
    exibeHoraDataTemperaturaHumidade();
    //  alarme();
    // ldrSerial();

  }

}
