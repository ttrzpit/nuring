#include "T_VIbrotactileClass.h"


// System data manager
#include "T_SharedDataManagerClass.h"


T_VibrotactileClass::T_VibrotactileClass( SharedDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {

	  };


void T_VibrotactileClass::Begin() {

	// Pins for vibrating motor
	// pinMode( VIBROTACTILE_PIN, OUTPUT );
	pinMode( LED_BUILTIN, OUTPUT );
	pinMode( VIBROTACTILE_PIN, OUTPUT );

	analogWrite( LED_BUILTIN, 0 );
	analogWrite( VIBROTACTILE_PIN, 0 );
}

void T_VibrotactileClass::Drive() {

	if ( shared->Vibration.isRunning ) {
		analogWrite( LED_BUILTIN, 200 );
		analogWrite( VIBROTACTILE_PIN, 200 );
	} else {
		analogWrite( LED_BUILTIN, 0 );
		analogWrite( VIBROTACTILE_PIN, 0 );
	}
}