#ifndef Z21_H
#define Z21_H

#include <Z21Observer.h>

#define MaxLocoAddr 9999

#define Z21_PORT 21105
#define Z21_DEFAULT_ADDR "192.168.0.111"

// Lebenszeichen (alle ... Millisekunden Zentrale adressieren, um Verbindung aufrechtzuerhalten
#define Z21_HEARTBEAT 30000

#define yetUnknown "(noch nicht ausgelesen)" // bestimmte Zustände

#include <Arduino.h>
#include <WiFiUDP.h>
#include <WiFi.h>

#define MaxObservers 20
#define MaxFct 31

class Z21 {

  public:
  
	// Empfang von Z21-Meldungen
	static void receive();

	// Observer hinzufügen, der dann über Ereignisse informiert wird
    // Er muss das Z21Observer-Interface implementieren
    static void addObserver(Z21Observer* observer) {
    	if (numObservers >= MaxObservers) return;
        observers[numObservers++] = observer;
    }

	// Regelmäßig an Z21 zu sendende Nachricht, um Verbindung aufrechtzuerhalten
	static uint16_t heartbeat();
	
	// Anfangs zu sendende Anfragen 
	static void init();

	// Broadcast Flags setzen
	static void LAN_SET_BROADCASTFLAGS(char f1, char f2, char f3, char f4);
	
	// Broadcastflags abfragen
	static void LAN_GET_BROADCASTFLAGS();
	
	// Systemzustand abfragen
	static void LAN_SYSTEMSTATE_GETDATA();
	
	// Gleisspannungszustand abfragen
	static BoolState getTrackPowerState() { return trackPowerState; }

	// Nothaltzustand abfragen
	static BoolState getEmergencyStopState() { return emergencyStopState; }

	// Kurzschlusszustand abfragen
	static BoolState getShortCircuitState() { return shortCircuitState; }

	// Programmierstatus abfragen
	static BoolState getProgState() { return progState; }

	// Seriennummer abfragen
	static void LAN_GET_SERIAL_NUMBER();
	
	// HW-Info abfragen
	static void LAN_GET_HWINFO();
	
	// Firmwareversion abfragen
	static void LAN_X_GET_FIRMWARE_VERSION();
	
	// Version abfragen
	static void LAN_X_GET_VERSION();
	
	// Status abfragen
	static void LAN_X_GET_STATUS();
	
	// Gleisspannung ein/aus
	static void LAN_X_SET_TRACK_POWER(boolean on);

	// Not-Halt
	static void LAN_X_SET_STOP() ;

	// Zubehör schalten
	static void LAN_X_SET_TURNOUT(int addr, bool plus);
	
	// Zubehör abfragen
	static void LAN_X_GET_TURNOUT_INFO(int addr);

	// Lok mit Adresse <adr> fahren in Richtung <dir> und mit Geschwindigkeit <speed>
	static void LAN_X_SET_LOCO_DRIVE(int addr, Direction dir, int speed);
	
	
	static void locoStop(int addr, Direction dir);
	
	// Lokfunktion setzen
	static void LAN_X_SET_LOCO_FUNCTION(int addr, int func, bool plus);
	
	// Lokinfo abfragen
	static void LAN_X_GET_LOCO_INFO(int addr);
	  
	// POM Write
	static void LAN_X_CV_POM_WRITE_BYTE(int addr, int cv, byte value);
	  
	// CV Write
	static void LAN_X_CV_WRITE(int cv, byte value);
	
	// CV Read
	static void LAN_X_CV_READ(int cv);
	
	static void setIPAddress(String ipAddr);
	static String getIPAddress();

	static void setAddrOffs(int offs) {addrOffs = offs; }


	static String toString(BoolState state, String onLabel = "Ein", String offLabel = "Aus");

	static String ipAddress;
	
	static String xbusVersion;
	static String csID;
	static String serialNbr;
	static String fwVersion;
	static String currMain;
	static String hwVersion;
	
	static String currProg;
	static String temp;
	static bool lowVoltage;
	static bool highTemp;

	static long lastReceived;
	
	static boolean flagsValid;
	static boolean BCF_BASIC;	
	static boolean BCF_RBUSFB;	
	static boolean BCF_RC;	
	static boolean BCF_SYS;	
	static boolean BCF_ALLLOCO;	
	static boolean BCF_ALLRC;	
	static boolean BCF_CANFB;	
	static boolean BCF_LNMSG;	
	static boolean BCF_LNLOCO;	
	static boolean BCF_LNSwitch;	
	static boolean BCF_LNFB;
	  
	private:

		// Leiten entsprechende Notifikationen an alle Observer weiter
		static void notifyTrackPowerStateChanged(BoolState trackPowerState);
		static void notifyShortCircuitStateChanged(BoolState shortCircuitState);
		static void notifyEmergencyStopStateChanged(BoolState emergencyStopState);
		static void notifyProgStateChanged(BoolState progState);
		static void notifyProgResult(ProgResult result, int value);
		static void notifyLocoInfoChanged(int addr, Direction dir, int fst, bool takenOver, int numSpeedSteps, bool f[]);
		static void notifyAccessoryStateChanged(int addr, bool plus);
		static void notifyTraceEvent(FromToZ21 direction, long diffLastSentReceived, String message, String parameters);

		// Umrechnung von Benutzer gesehener Fst 0..126 in DCC-Fst 0..127 (unsägliche Fst 1 == Nothalt!!!)
		static int userToDccSpeed(int userSpeed);
		static int dccToUserSpeed(int dccSpeed);

		// Interne Setter (einschl. nachgelagerte Notifikation) - beeinflussen die Z21 nicht!

		static BoolState trackPowerState;
		static void setTrackPowerState(BoolState trackPowerState);

	  	static BoolState progState;
		static void setProgState(BoolState progState); 

		static BoolState emergencyStopState;
		static void setEmergencyStopState(BoolState emergencyStopState);

		static BoolState shortCircuitState;
		static void setShortCircuitState(BoolState shortCircuitState);

		static void locoDrive(int addr, Direction dir, int speed);

		static void sendCommand(byte bytes[], int len, bool addXOR);
		static void insertChecksum(byte bytes[], int len); 
		static int lastControlledAddress;

		static int addrOffs;

		// ---------- Anzahl und Liste der Observer 
        static int numObservers;
        static Z21Observer* observers[MaxObservers];

		static void trace(FromToZ21 direction, long diffLastSentReceived, String message, String parameters);


};


#endif
