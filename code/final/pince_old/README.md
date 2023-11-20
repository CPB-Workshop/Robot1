# Code Final - Option Pince

Si vous avez décidé de passer par le code final, il faut alors flasher l'arduino (toujours bluetooth débranché) avec le code fournis (pince.ino)


Pour le controller par bluetooth via votre smartphone android, il faut télécharger l'application Bluetooth Electronics de Keuwlsoft

<img src="appliFlashCode.png" style="margin-left: 50%;" />

Ensuite importer le fichier "CPBRobotPince.kwl" pour obtenir un dashboard qui va permettre de commander la machine (ne pas oublier de rebrancher le bluetooth ;) )


# TROUBLESHOOTING

## Robot qui avant pas droit ou recule
En cas de probleme de sens des moteurs (le robot tourne au lieux de rouler droit/recule) ont peut modifier le code a loisir sur la partie suivante :

```C++
    void lancerRotationMoteurPontA(bool marcheAvant, char vitesse) {
        digitalWrite(borneIN2, marcheAvant ? LOW : HIGH);    // inversion LOW et HIGH, Idem en dessous pour inverser le comportement d'une roue            
        digitalWrite(borneIN1, marcheAvant ? HIGH : LOW);
        
        analogWrite(borneENA, vitesse);       // Active l'alimentation du moteur 1
    }
    //[...]
    void lancerRotationMoteurPontB(bool marcheAvant, char vitesse) {
        digitalWrite(borneIN3, marcheAvant ? LOW : HIGH);    // inversion LOW et HIGH, Idem en dessous pour inverser le comportement d'une roue            
        digitalWrite(borneIN4, marcheAvant ? HIGH : LOW);
        
        analogWrite(borneENB, vitesse);       // Active l'alimentation du moteur 2
    }
```