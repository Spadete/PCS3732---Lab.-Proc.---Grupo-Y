/*
  Calculadora 4 bits - Complemento de 1
  Entrada e saída apenas pela Serial
*/

int numeroParaComp1(int n) {
  if (n >= 0) {
    return n;
  }

  // complemento de 1 para negativos
  return (~(-n)) & 0x0F;
}

String bin4(int valor) {
  String s = "";

  for (int i = 3; i >= 0; i--) {
    s += String((valor >> i) & 1);
  }

  return s;
}

void setup() {
  Serial.begin(115200);

  while (!Serial);

  Serial.println("=== Calculadora 4 bits ===");
  Serial.println("Complemento de 1");
  Serial.println("Intervalo: -7 ate 7");
}

void loop() {

  Serial.println("\nDigite o primeiro numero:");
  while (!Serial.available());

  int a = Serial.parseInt();

  while (Serial.available()) Serial.read();

  Serial.println("Digite a operacao (+ ou -):");
  while (!Serial.available());

  char op = Serial.read();

  while (Serial.available()) Serial.read();

  Serial.println("Digite o segundo numero:");
  while (!Serial.available());

  int b = Serial.parseInt();

  while (Serial.available()) Serial.read();

  int resultado = 0;

  if (op == '+') {
    resultado = a + b;
  }
  else if (op == '-') {
    resultado = a - b;
  }
  else {
    Serial.println("Operacao invalida.");
    return;
  }

  // overflow simples para 4 bits
  bool overflow = false;

  if (resultado > 7 || resultado < -7) {
    overflow = true;
  }

  int binA = numeroParaComp1(a);
  int binB = numeroParaComp1(b);
  int binR = numeroParaComp1(resultado);

  Serial.println("\n===== RESULTADO =====");

  Serial.print("A = ");
  Serial.print(a);
  Serial.print(" -> ");
  Serial.println(bin4(binA));

  Serial.print("B = ");
  Serial.print(b);
  Serial.print(" -> ");
  Serial.println(bin4(binB));

  Serial.print("Resultado = ");
  Serial.print(resultado);
  Serial.print(" -> ");
  Serial.println(bin4(binR));

  if (overflow) {
    Serial.println("OVERFLOW!");
  }

  delay(1000);
}