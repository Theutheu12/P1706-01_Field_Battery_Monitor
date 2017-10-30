//######################################################################################
//SIM900 - Envoyer des SMS
//V.Theurillat
//######################################################################################
//######################################################################################
#include <SoftwareSerial.h>
#include <Cmd.h>
//######################################################################################
//######################################################################################
#define onModulePin 9
//######################################################################################
//######################################################################################
SoftwareSerial mySerial(7,8);
int8_t	answer;
char 	aux_string[30];
char phone_number[]="+41798216349";
//######################################################################################

// Cette fonction permet d'envoyer des commandes AT au module GSM.
int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    // Initialisation de la chaine de caractère (string).
    memset(response, '\0', 100);
    
    delay(100);
    
    // Initialisation du tampon d'entrée (input buffer).
    while( mySerial.available() > 0) mySerial.read();
    
    // Envoi des commandes AT
    mySerial.println(ATcommand);


    x = 0;
    previous = millis();

    // Cette boucle attend la réponse du module GSM.
    
    do{
// Cette commande vérifie s'il y a des données disponibles dans le tampon.
//Ces données sont comparées avec la réponse attendue.
        if(mySerial.available() != 0){    
            response[x] = mySerial.read();
            x++;
            // Comparaison des données
            if (strstr(response, expected_answer) != NULL)    
            {
                answer = 1;
            }
        }
    // Attente d'une réponse.
    }while((answer == 0) && ((millis() - previous) < timeout));    

    //Serial.println(response); //Cette ligne permet de debuguer le programme en cas de problème !
    return answer;
}

void power_on(){

    uint8_t answer=0;
    
    // Cette commande vérifie si le module GSM est en marche.
    answer = sendATcommand("AT", "OK", 2000);
    Serial.println(answer, DEC);
    if (answer == 0)
    {
        // Mise en marche du module GSM
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);
    
        // Envoie d'une commande AT toutes les deux secondes et attente d'une réponse.
        while(answer == 0){
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }
    
}




void setup() {

	pinMode(onModulePin, OUTPUT);
    Serial.begin(115200);
    mySerial.begin(115200);  
        
    Serial.println("------------------------------------------------------");
    Serial.println("----Upsilon Audio - Envoyer un SMS avec le SIM900-----");
    Serial.println("------------------------------------------------------");
    Serial.println("");
    Serial.println("Initialisation en cours ...");
    power_on();
    
    //delay(3000);
    
    // Cette commande active la carte SIM.
    //Supprimez cette ligne si vous n'avez pas de code PIN.
    //sendATcommand("AT+CPIN=****", "OK", 2000);
    
    delay(3000);
    
    Serial.println("Connexion au reseau en cours ...");

    while( (sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || 
            sendATcommand("AT+CREG?", "+CREG: 0,5", 500)) == 0 );

    Serial.println("Mode SMS en cours d'activation ...");
    // Activation du mode texte pour les SMS.
    sendATcommand("AT+CMGF=1", "OK", 1000);
    Serial.println("Envoi du SMS en cours ...");
    
    sprintf(aux_string,"AT+CMGS=\"%s\"", phone_number);
    // Envoi du numéro de téléphone au module GSM.
    answer = sendATcommand(aux_string, ">", 2000);
    if (answer == 1)
    {
        // Insérez ici le coprs du message.
        mySerial.println("Ceci est un SMS !");
        mySerial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
            Serial.println("Message envoye !");    
        }
        else
        {
            Serial.print("Erreur !");
        }
    }
    else
    {
        Serial.print("Erreur !");
        Serial.println(answer, DEC);
    }

}

void loop() {
  // put your main code here, to run repeatedly:
}