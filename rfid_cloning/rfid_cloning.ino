//This is where Minzah and Paige are gonna work
  #include <SPI.h>
  
  char choice;
  char subchoice;
  
void setup() {
   Serial.begin(9600);
   while (!Serial);            
   SPI.begin();
   Serial.println(F("0.Read \n1.Manipulate Data Blocks \n2.Clone"));

}

void loop() {
  displayMenu();
}

void displayMenu(){
  //Serial.print("0.Read\n1.Manipulate Data Blocks\n2.Clone");
  
  choice = Serial.read();

  if( choice == '0'){ 
    choice0();
  }
  else if (choice == '1'){ 
    choice1();
  }
  else if (choice == '2'){
    choice2();
  }    
}

void choice0(){
  Serial.print("UID and data");
}

void choice1(){
  Serial.print("0.Write \n1.Erase");
  subchoice = Serial.read();
  Serial.print("Hello");

  if (subchoice == '0'){
    subchoice0();
  }
  else if (subchoice == '1'){
    subchoice1();
  }
}
void choice2(){
  Serial.print("Cloning instructions");
}

void subchoice0(){
  Serial.print("Data to be written");
}

void subchoice1(){
  Serial.print("Data to be erased"); 
}
