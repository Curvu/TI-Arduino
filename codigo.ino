const byte yesPin = 3, noPin = 4;
const int btnDelay = 50;
unsigned long longPressDur = 2000;
int yesBtnLstState = LOW, noBtnLstState = LOW;
bool btnLongPress = false;
unsigned long longPressMillis, lstBtnMillis, pressDur, crntMillis;
byte j = 0, x = 0, num = 0;

void mostrarSequencia(byte j) // Mostra as sequencias
{
  byte count = 0;
  Serial.println(">> Sequencia:");
  for(byte i = 0; i <= 127; i++) // ciclo de 0 até 127
  {
    if(i >> j & 0b1) // Se o numero em binario (i) tiver na sua posição (j) o numero 1 (ou é 1 ou 0) então faz a função 
    {
      Serial.print(i); Serial.print(" "); // vai mostrando os numeros
      count++; // vai aumentando o contador
      if((count %= 25) == 0) Serial.println(""); // quando o contador chegar a 25 reseta e fica a 0, quando está a 0 dá lineBreak (muda de linha)
    }
  }
  Serial.println("\n> O numero esta nesta sequencia?");
}

void desligarLeds() // Desligar todos os leds
{
  for(int i = 0; i <= 7; i++) digitalWrite(6+i, LOW);
}

void varrimentoLeds() // Varrimento dos Leds
{
  for(int i = 0; i < 7; i++)
  {
    if((i-1) < 0) digitalWrite(6+i, HIGH); // liga o 1º led
    else if(((i-1) >= 0) && (i < 7))
    {
      digitalWrite(6+i, HIGH); // liga led (6+i), em que i é um numero inteiro
      digitalWrite(6+i-1, LOW); // desliga o led da posição anterior
    }
    delay(300);
  }
  digitalWrite(12, LOW); // desliga o ultimo led
}

void ligarLedsBinario(byte num) // Calcula o Numero Binario e acende os leds respetivos á posição do 1
{
  // mostrar o num atual calculado por leds em biário
  for(byte i = 0; i < 7; i++){
    bool res = (num >> i) & 0b1; // ex:. (0010 >> 1) & 0b1 = 0001 & 0b1 = 0, logo o valor res é false, então não acende led
    if(res) digitalWrite(6+i, HIGH); // Se o valor do bool res for verdadeiro, acende o (6+i) led
  }
}

void setup() // SETUP HERE
{
  Serial.begin(9600);
  for(int i = 6; i <= 12; i++) pinMode(i, OUTPUT); // liga pins do 6 até ao 12
  pinMode(yesPin, INPUT_PULLUP); // liga pin 4 (para o botão yes)
  pinMode(noPin, INPUT_PULLUP); // liga pin 3 (para o botão no)
}

void loop() // LOOP HERE
{
  int yesBtnState = !digitalRead(yesPin); // vê o estado atual do botão 
  int noBtnState = !digitalRead(noPin); // vê o estado atual do botão
  crntMillis = millis(); // vê o tempo atual de que o arduino está ligado
  
  if (crntMillis - lstBtnMillis > btnDelay) // permite que o botão seja clicado uma unica vez
  {
    // Vê se um dos botões foi premido
    if (yesBtnState && !yesBtnLstState && !btnLongPress) // estado do botão 'yes' ativo e ultimo estado desativo e não é longPress
    {
      longPressMillis = crntMillis; // vai igualando a variavel de longPressMillis ao millis atual
      yesBtnLstState = HIGH; // o ultimo estado do botão fica HIGH (ativo)
    }
    else if (noBtnState && !noBtnLstState && !btnLongPress) // estado do botão 'no' ativo e ultimo estado desativo e não é longPress
    {
      longPressMillis = crntMillis; // vai igualando a variavel de longPressMillis ao millis atual
      noBtnLstState = !noBtnLstState; // o ultimo estado do botão fica negado, ou seja HIGH (ativo)
    }
    
    pressDur = crntMillis - longPressMillis; // vê quanto tempo o botão está ativo e guarda na variavel pressDur
    // Vê se o botão foi premido durante 2 segundos
    if ((yesBtnState || noBtnState) && !btnLongPress && pressDur >= longPressDur) // estado do botão ativo (botão - yes)
    {
      btnLongPress = true; // O botão está premido á 2 ou mais segundos
      desligarLeds(); // Chama a função para desligar os leds
      j = 0; x = 0; num = 0; // Reseta as variaveis ´j´, ´x´ e ´num´
      Serial.println(">>> RESET");
    }
    
    // Vê quando se larga o botão, quando o estado estiver LOW (desativo) e o ultimo estado HIGH (ativo)
    if (!yesBtnState && yesBtnLstState) // Botão YES solto
    {
      // reseta as variaveis do ultimo estado do botão yes e de longPress
      yesBtnLstState = !yesBtnLstState;
      btnLongPress = false;
      
      // Aparece na primeira vez que se larga o botão
      if (x == 0) Serial.println(">>> Pense num numero entre 0 e 127");
      x++; // adiciona 1 á variavel `x` de modo a que nenhuma das funções anteriores se repita
      
      if (j < 7) mostrarSequencia(j); // variavel j menor que 7, então chama a função
      
      if ((j - 1) < 0) j = 1; // Se (j-1) < 0 , iguala j a 1, para que var << (j-1) = 1
      
      if (x == 1) num = 0; // Reseta a variavel num (se ligar o 'jogo' no botão yes, não começar com o led do pin 6 aceso)
      else if (x > 1)
      {
        num = num + (1 << j-1); // var << j-1, faz com que o bit que está a 1, ande (j-1) casas para a esquerda, e vai ficando com os valores 1,2,4,8 etc.
        j++; // adiciona 1 á variavel j
      }
    }
    else if (!noBtnState && noBtnLstState) // Botão NO solto
    {
      // reseta as variaveis do ultimo estado do botão no e de longPress
      noBtnLstState = !noBtnLstState; btnLongPress = false;
      
      // Aparece na primeira vez que se larga o botão
      if (x == 0) Serial.println(">>> Pense num numero entre 0 e 127");
      x++; // adiciona 1 á variavel `x` de modo a que nenhuma das funções anteriores se repita
      
      if (j < 7) mostrarSequencia(j); // variavel j menor que 7, então chama a função
      j++; // adiciona 1 á variavel j
    }
    lstBtnMillis = crntMillis; // iguala o ultimo millis ao millis atual
    ligarLedsBinario(num); //
  }
  
  if (j == 8) // faz quando a variavel j chegar a 8
  {
    Serial.print(">>> O numero secreto e: "); Serial.println(num); // mostra o numero secreto
    delay(5000); // delay 5 segundos antes do varrimento
    desligarLeds(); // Chama a função para desligar os leds
    varrimentoLeds(); // Chama a função para ligar e desligar os leds sucessivamente (varrimento)
    j = 0; x = 0; num = 0; // reseta as variaveis ´j´, ´x´ e ´num´
  }
}
