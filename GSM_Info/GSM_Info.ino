//SIM900 - Prise en main
//Par Pierre Pelé
//http://upsilonaudio.com/
//Décembre 2013


#include <SoftwareSerial.h>

SoftwareSerial SIM900_Serial(7,8);

int8_t answer;
int x ;
int onModulePin= 2; // Il s'agit du port DTR (ou PWR) du module SIM900.
char REPONSE[200];

void setup(){

    pinMode(onModulePin, OUTPUT);
    SIM900_Serial.begin(19200);
    Serial.begin(19200);
    Serial.println("------------------------------------------------------");
    Serial.println("----Upsilon Audio - Prise en main du module SIM900----");
    Serial.println("------------------------------------------------------");
    Serial.println("");
    Serial.println("Initialisation en cours ...");
    Serial.println("");
    //power_on();
    delay(3000);
    Serial.println("------------------------------------------------------");
    Serial.println("Nom de votre operateur");
    sendATcommand_2("AT+COPS?","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Etat de la connexion au reseau");
    Serial.println("La connexion est etablie si +CREG: 0,0 ou +CREG: 0,5");
    sendATcommand_2("AT+CREG?","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Numero de telephone");
    sendATcommand_2("AT+CNUM","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Numero IMSI");
    sendATcommand_2("AT+CIMI","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Services offerts par le modem");
    sendATcommand_2("AT+GCAP","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Qualitee du signal");
    sendATcommand_2("AT+CSQ","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Numero IMEI");
    sendATcommand_2("AT+CGSN","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Numero ICCID");
    sendATcommand_2("AT+CCID","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Charge de la batterie");
    sendATcommand_2("AT+CBC","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Nom du modele");
    sendATcommand_2("AT+GMM","OK",5000);
    Serial.println("------------------------------------------------------");
    Serial.println("Version du software");
    sendATcommand_2("AT+CGMR","OK",5000);
}


void loop() // Il n'y a aucune commandes dans la fonction loop.
{
}

// Définition des différentes fonctions.

void power_on(){

    uint8_t answer=0;
    
    // Cette commande vérifie si le module GSM est en marche.
    answer = sendATcommand_1("AT", "OK", 2000);
    if (answer == 0)
    {
        // Mise en marche du module GSM
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);
    
        // Envoie d'une commande AT toutes les deux secondes et attente d'une réponse.
        while(answer == 0){
            answer = sendATcommand_1("AT", "OK", 2000);    
        }
    }
    
}

// Cette fonction permet d'envoyer des commandes AT au module GSM.
int8_t sendATcommand_1(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    // Initialisation de la chaine de caractère (string).
    memset(response, '\0', 100);
    
    delay(100);
    
    // Initialisation du tampon d'entrée (input buffer).
    while( SIM900_Serial.available() > 0) SIM900_Serial.read();
    
    // Envoi des commandes AT
    SIM900_Serial.println(ATcommand);


    x = 0;
    previous = millis();

    // Cette boucle attend la réponse du module GSM.
    
    do{
// Cette commande vérifie s'il y a des données disponibles dans le tampon.
//Ces données sont comparées avec la réponse attendue.
        if(SIM900_Serial.available() != 0){    
            response[x] = SIM900_Serial.read();
            x++;
            // Comparaison des données
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
            }
        }
    // Attente d'une réponse.
    }while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}

void sendATcommand_2(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    // Initialisation de la chaine de caractère (string).
    memset(response, '\0', 100);
    
    delay(100);
    
    // Initialisation du tampon d'entrée (input buffer).
    while( SIM900_Serial.available() > 0) SIM900_Serial.read();
    
    // Envoi des commandes AT
    SIM900_Serial.println(ATcommand);


    x = 0;
    
    
    previous = millis();

    // Cette boucle attend la réponse du module GSM.
    
    do{
// Cette commande vérifie s'il y a des données disponibles dans le tampon.
//Ces données sont comparées avec la réponse attendue.
        if(SIM900_Serial.available() != 0){    
            response[x] = SIM900_Serial.read();
            x++;
            // Comparaison des données
            if (strstr(response, expected_answer) != NULL)    
            {
              response[x] = '\0';  
              answer = 1;
            }
        }
    // Attente d'une réponse.
    }while((answer == 0) && ((millis() - previous) < timeout));    

    if (answer == 0){
      Serial.println("");
      Serial.println("Erreur ! Temps depasse.");
      Serial.println("");
    }
    else
    {
      Serial.println("");
      Serial.println(response);
      Serial.println("");
    }
}
