// Call to class header
#include "InputClass.h"
#include <chrono>
#include <iomanip>	  // For padding zeroes
#include <thread>

// System data manager
#include "SystemDataManager.h"

#include "FittsClass.h"

// Constructor
InputClass::InputClass( SystemDataManager& ctx )
	: dataHandle( ctx )
	, shared( ctx.getData() ) {

	// Register key bindings
	RegisterKeyBindings();

	std::cout << "InputClass:   Input initialized.\n";
}


void InputClass::ParseInput( int key ) {

	// Ensure only keyboard input accepted
	if ( key != 255 ) {

		// Uncomment to be able to read detected key presses
		std::cout << "InputClass:   Detected key press, index = " << key << "\n";

		// Check if binding present
		if ( keyBindings.count( key ) ) {

			// Run bound function
			keyBindings[key]();
		}
	}
}



// Functions
void InputClass::IncrementValueF( std::string name, float& target, float increment ) {

	if ( ( target + increment ) >= 0 ) {
		// Increment
		target = target + increment;
	}

	// Update
	shared->Display.statusString = "InputClass: " + name + ( ( increment > 0 ) ? " increased to " : " decreased to " ) + shared->FormatDecimal( target, 1, 2 );
}



/*
 *
 *
 * 
 *
 * 
 * 
 *  
 *  =========================================================================================
 *  ========================================================================================= 
 * 
 *   BBBBBBB    IIIII   NNN   NN  DDDDDDD   IIIII  NNN   NN  GGGGGGG   SSSSS
 *   BB   BBB    III    NNNN  NN  DD   DDD   III   NNNN  NN  GG       SS
 *   BB   BBB    III    NN NN NN  DD   DDD   III   NN NN NN  GG       SS
 *   BBBBBBB     III    NN  NNNN  DD   DDD   III   NN  NNNN  GG  GGG   SSSSS
 *   BB   BBB    III    NN   NNN  DD   DDD   III   NN   NNN  GG   GG       SS
 *   BB   BBB    III    NN   NNN  DD   DDD   III   NN   NNN  GG   GG       SS
 *   BBBBBBB    IIIII   NN   NNN  DDDDDDD   IIIII  NN   NNN   GGGGG   SSSSSS
 * 
 *  ========================================================================================= 
 *  ========================================================================================= 
 */



/** KEY BINDINGS */

void InputClass::RegisterKeyBindings() {
	keyBindings[27]	 = [this]() { K_Exit(); };
	keyBindings['1'] = [this]() { K_SetActive1(); };
	keyBindings['2'] = [this]() { K_SetActive2(); };
	keyBindings['3'] = [this]() { K_SetActive3(); };
	keyBindings['4'] = [this]() { K_SetActive4(); };
	keyBindings['5'] = [this]() { K_SetActive5(); };
	keyBindings['`'] = [this]() { K_SetActiveNone(); };
	keyBindings['a'] = [this]() { K_AmplifierToggle(); };
	keyBindings['t'] = [this]() { K_AmplifierTensionToggle(); };
	keyBindings['s'] = [this]() { K_SerialToggle(); };
	keyBindings[180] = [this]() { K_DirSelect_Abduction(); };
	keyBindings[182] = [this]() { K_DirSelect_Adduction(); };
	keyBindings[178] = [this]() { K_DirSelect_Flexion(); };
	keyBindings[184] = [this]() { K_DirSelect_Extension(); };
	keyBindings['p'] = [this]() { K_GainSelect_Proportional(); };
	keyBindings['i'] = [this]() { K_GainSelect_Integral(); };
	keyBindings['d'] = [this]() { K_GainSelect_Derivative(); };
	keyBindings[181] = [this]() { K_GainsZero(); };
	keyBindings[61]	 = [this]() { K_TenSelect_A(); };
	keyBindings[175] = [this]() { K_TenSelect_B(); };
	keyBindings[170] = [this]() { K_TenSelect_C(); };
	keyBindings[171] = [this]() { K_Increment(); };
	keyBindings[173] = [this]() { K_Decrement(); };
	keyBindings['z'] = [this]() { K_EncoderZero(); };
	keyBindings['x'] = [this]() { K_EncoderMeasureLimit(); };
	keyBindings['c'] = [this]() { K_EncoderSetLimit(); };
	keyBindings['f'] = [this]() { K_FittsStart(); };
	keyBindings['g'] = [this]() { K_FittsStop(); };
	keyBindings[196] = [this]() { K_TaskCalibrationStart(); };
}



/** 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 *  =========================================================================================
 *  ========================================================================================= 
 * 
 *   CCCCCCC    AAAA    LL       LL       BBBBBBBB     AAAA     CCCCCCC  KK    KK    SSSSS
 *  CC         AA  AA   LL       LL       BB     BB   AA  AA   CC        KK   KK   SS
 *  CC         AA  AA   LL       LL       BB     BB   AA  AA   CC        KK KK     SS
 *  CC        AAAAAAAA  LL       LL       BBBBBBBB   AAAAAAAA  CC        KKKK        SSSSS
 *  CC        AA    AA  LL       LL       BB     BB  AA    AA  CC        KK KK            SS
 *  CC        AA    AA  LL       LL       BB     BB  AA    AA  CC        KK  KK           SS
 *   CCCCCCC  AA    AA  LLLLLLL  LLLLLLL  BBBBBBBB   AA    AA   CCCCCCC  KK   KK    SSSSSS
 * 
 *  ========================================================================================= 
 *  ========================================================================================= 
 */


/** KEYBOUND FUNCTIONS */

void InputClass::SafeDecrement( float& target, float decrement ) {

	if ( target >= abs( decrement ) ) {
		target += decrement;
	}
}


// Exit program
void InputClass::K_Exit() {
	shared->Display.statusString		= "InputClass: Shutting down...";
	shared->System.state				= stateEnum::IDLE;
	shared->Amplifier.isAmplifierActive = false;
	shared->Serial.isSerialSending		= false;
	shared->Serial.isSerialSendOpen		= false;
	cv::destroyAllWindows();
	std::cout << "Shutdown initiated.\n";
	shared->System.isShuttingDown = true;
}

void InputClass::K_SetActive1() {
	shared->Telemetry.activeID	 = 1;
	shared->Display.statusString = "InputClass: Updated active marker to #" + shared->Telemetry.activeID;
}

void InputClass::K_SetActive2() {
	shared->Telemetry.activeID	 = 2;
	shared->Display.statusString = "InputClass: Updated active marker to #" + shared->Telemetry.activeID;
}

void InputClass::K_SetActive3() {
	shared->Telemetry.activeID	 = 3;
	shared->Display.statusString = "InputClass: Updated active marker to #" + shared->Telemetry.activeID;
}

void InputClass::K_SetActive4() {
	shared->Telemetry.activeID	 = 4;
	shared->Display.statusString = "InputClass: Updated active marker to #" + shared->Telemetry.activeID;
}

void InputClass::K_SetActive5() {
	shared->Telemetry.activeID	 = 5;
	shared->Display.statusString = "InputClass: Updated active marker to #" + shared->Telemetry.activeID;
}

void InputClass::K_SetActiveNone() {
	shared->Telemetry.activeID	 = 0;
	shared->Display.statusString = "InputClass: Updated active marker to #" + shared->Telemetry.activeID;
}

void InputClass::K_AmplifierToggle() {

	// Toggle
	shared->Amplifier.isAmplifierActive = !shared->Amplifier.isAmplifierActive;

	if ( shared->Amplifier.isTensionOnly ) {
		shared->Amplifier.isAmplifierActive = true;
		shared->Amplifier.isTensionOnly		= false;
	}

	if ( shared->Amplifier.isAmplifierActive ) {
		shared->Telemetry.isTargetReset	  = true;
		shared->Controller.isRampingUp	  = true;
		shared->Controller.rampPercentage = 0.0f;
		shared->Controller.rampStartTime  = shared->Timing.elapsedRunningTime;
		shared->System.state			  = stateEnum::DRIVING_PWM;
		shared->Display.statusString	  = "InputClass: Amplifiers enabled.";
	} else {
		// shared->Serial.packetOut = "DX\n";
		// shared->FLAG_PACKET_WAITING = true;
		shared->System.state		 = stateEnum::IDLE;
		shared->Display.statusString = "InputClass: Amplifiers disabled.";
	}
}

void InputClass::K_AmplifierTensionToggle() {

	// If tension disabled, enable
	if ( !shared->Amplifier.isTensionOnly ) {

		shared->Amplifier.isTensionOnly		= true;
		shared->Amplifier.isAmplifierActive = true;
		shared->System.state				= stateEnum::DRIVING_PWM;

	}
	// If tension enabled, disable
	else {

		shared->Amplifier.isTensionOnly		= false;
		shared->Amplifier.isAmplifierActive = false;
		// if ( shared->System.state != stateEnum::IDLE ) {
		// 	shared->System.state = stateEnum::IDLE;
		// }
	}

	shared->Display.statusString = "InputClass: Tension-only is " + std::string( shared->Amplifier.isTensionOnly ? "enabled." : "disabled." );
}



void InputClass::K_SerialToggle() {

	// Toggle serial flags
	shared->Serial.isSerialSending	 = !shared->Serial.isSerialSending;
	shared->Serial.isSerialReceiving = !shared->Serial.isSerialReceiving;

	if ( shared->Serial.isSerialSending ) {
		shared->Display.statusString	 = "InputClass: Enabling serial IO.";
		shared->Serial.isSerialSending	 = true;
		shared->Serial.isSerialReceiving = true;
	} else {
		shared->Serial.isSerialSending	 = false;
		shared->Serial.isSerialReceiving = false;
		shared->Display.statusString	 = "InputClass: Disabling serial IO.";
	}
}

void InputClass::K_DirSelect_Abduction() {
	shared->Input.selectGainTarget = ( shared->Input.selectGainTarget == selectGainTargetEnum::ABD ) ? selectGainTargetEnum::NONE : selectGainTargetEnum::ABD;
	shared->Display.statusString   = "InputClass: Selected abduction axis...";
}

void InputClass::K_DirSelect_Adduction() {
	shared->Input.selectGainTarget = ( shared->Input.selectGainTarget == selectGainTargetEnum::ADD ) ? selectGainTargetEnum::NONE : selectGainTargetEnum::ADD;
	shared->Display.statusString   = "InputClass: Selected adduction axis...";
}

void InputClass::K_DirSelect_Flexion() {
	shared->Input.selectGainTarget = ( shared->Input.selectGainTarget == selectGainTargetEnum::FLEX ) ? selectGainTargetEnum::NONE : selectGainTargetEnum::FLEX;
	shared->Display.statusString   = "InputClass: Selected flexion axis...";
}

void InputClass::K_DirSelect_Extension() {
	shared->Input.selectGainTarget = ( shared->Input.selectGainTarget == selectGainTargetEnum::EXT ) ? selectGainTargetEnum::NONE : selectGainTargetEnum::EXT;
	shared->Display.statusString   = "InputClass: Selected extension axis...";
}

void InputClass::K_TenSelect_A() {
	shared->Input.selectGainTarget = ( shared->Input.selectGainTarget == selectGainTargetEnum::AMPA ) ? selectGainTargetEnum::NONE : selectGainTargetEnum::AMPA;
	shared->Input.selectGain	   = selectGainEnum::NONE;
	shared->Display.statusString   = "InputClass: Selected tension A gain...";
	shared->Vibration.isRunning	   = !shared->Vibration.isRunning;
}

void InputClass::K_TenSelect_B() {
	shared->Input.selectGainTarget = ( shared->Input.selectGainTarget == selectGainTargetEnum::AMPB ) ? selectGainTargetEnum::NONE : selectGainTargetEnum::AMPB;
	shared->Input.selectGain	   = selectGainEnum::NONE;
	shared->Display.statusString   = "InputClass: Selected tension B gain...";
}

void InputClass::K_TenSelect_C() {
	shared->Input.selectGainTarget = ( shared->Input.selectGainTarget == selectGainTargetEnum::AMPC ) ? selectGainTargetEnum::NONE : selectGainTargetEnum::AMPC;
	shared->Input.selectGain	   = selectGainEnum::NONE;
	shared->Display.statusString   = "InputClass: Selected tension C gain...";
}


void InputClass::K_GainSelect_Proportional() {
	shared->Input.selectGain	 = selectGainEnum::KP;
	shared->Display.statusString = "InputClass: Selected proportional gain...";
}

void InputClass::K_GainSelect_Integral() {
	shared->Input.selectGain	 = selectGainEnum::KI;
	shared->Display.statusString = "InputClass: Selected integral gain...";
}

void InputClass::K_GainSelect_Derivative() {
	shared->Input.selectGain	 = selectGainEnum::KD;
	shared->Display.statusString = "InputClass: Selected derivative gain...";
}

void InputClass::K_GainsZero() {

	shared->Controller.gainKp.abd = 0.0f;
	shared->Controller.gainKp.add = 0.0f;
	shared->Controller.gainKp.flx = 0.0f;
	shared->Controller.gainKp.ext = 0.0f;
	shared->Controller.gainKi.abd = 0.0f;
	shared->Controller.gainKi.add = 0.0f;
	shared->Controller.gainKi.flx = 0.0f;
	shared->Controller.gainKi.ext = 0.0f;
	shared->Controller.gainKd.abd = 0.0f;
	shared->Controller.gainKd.add = 0.0f;
	shared->Controller.gainKd.flx = 0.0f;
	shared->Controller.gainKd.ext = 0.0f;
	shared->Display.statusString  = "InputClass: Gains set to zero.";
}

void InputClass::K_Increment() {

	// Handler for gains
	auto& gains = shared->Controller;

	// Increment value
	float increment = 0.0f;

	// Select appropriate increment for PID gains
	if ( shared->Input.selectGain == selectGainEnum::KP ) {
		increment = 0.1f;

	} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
		increment = 0.1f;

	} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
		increment = 0.01f;

	} else if ( shared->Input.selectGain == selectGainEnum::NONE ) {
		increment = 0.0f;

	} else {
		increment = 0.01f;
	}

	// Select appropriate increment for tension gains
	if ( shared->Input.selectGainTarget == selectGainTargetEnum::AMPA || shared->Input.selectGainTarget == selectGainTargetEnum::AMPB || shared->Input.selectGainTarget == selectGainTargetEnum::AMPC ) {
		increment = 0.01;
	}

	switch ( shared->Input.selectGainTarget ) {

		// Abduction
		case selectGainTargetEnum::ABD: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				gains.gainKp.abd += increment;
				shared->Display.statusString = "InputClass: Abduction Kp increased to " + shared->FormatDecimal( shared->Controller.gainKp.abd, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				gains.gainKi.abd += increment;
				shared->Display.statusString = "InputClass: Abduction Ki increased to " + shared->FormatDecimal( shared->Controller.gainKi.abd, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				gains.gainKd.abd += increment;
				shared->Display.statusString = "InputClass: Abduction Kd increased to " + shared->FormatDecimal( shared->Controller.gainKd.abd, 2, 2 );
			}
			break;
		}

		// Adduction
		case selectGainTargetEnum::ADD: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				gains.gainKp.add += increment;
				shared->Display.statusString = "InputClass: Adduction Kp increased to " + shared->FormatDecimal( shared->Controller.gainKp.add, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				gains.gainKi.add += increment;
				shared->Display.statusString = "InputClass: Adduction Ki increased to " + shared->FormatDecimal( shared->Controller.gainKi.add, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				gains.gainKd.add += increment;
				shared->Display.statusString = "InputClass: Adduction Kd increased to " + shared->FormatDecimal( shared->Controller.gainKd.add, 2, 2 );
			}
			break;
		}

		// Flexion
		case selectGainTargetEnum::FLEX: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				gains.gainKp.flx += increment;
				shared->Display.statusString = "InputClass: Flexion Kp increased to " + shared->FormatDecimal( shared->Controller.gainKp.flx, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				gains.gainKi.flx += increment;
				shared->Display.statusString = "InputClass: Flexion Ki increased to " + shared->FormatDecimal( shared->Controller.gainKi.flx, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				gains.gainKd.flx += increment;
				shared->Display.statusString = "InputClass: Flexion Kd increased to " + shared->FormatDecimal( shared->Controller.gainKd.flx, 2, 2 );
			}
			break;
		}

		// Extension
		case selectGainTargetEnum::EXT: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				gains.gainKp.ext += increment;
				shared->Display.statusString = "InputClass: Extension Kp increased to " + shared->FormatDecimal( shared->Controller.gainKp.ext, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				gains.gainKi.ext += increment;
				shared->Display.statusString = "InputClass: Extension Ki increased to " + shared->FormatDecimal( shared->Controller.gainKi.ext, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				gains.gainKd.ext += increment;
				shared->Display.statusString = "InputClass: Extension Kd increased to " + shared->FormatDecimal( shared->Controller.gainKd.ext, 2, 2 );
			}
			break;
		}


		// Amp A
		case selectGainTargetEnum::AMPA: {

			gains.commandedTensionABC.x += increment;
			shared->Display.statusString = "InputClass: Tension A increased to " + shared->FormatDecimal( shared->Controller.commandedTensionABC.x, 2, 2 );

			break;
		}

		// Amp B
		case selectGainTargetEnum::AMPB: {

			gains.commandedTensionABC.y += increment;
			shared->Display.statusString = "InputClass: Tension B increased to " + shared->FormatDecimal( shared->Controller.commandedTensionABC.y, 2, 2 );

			break;
		}

		// Amp C
		case selectGainTargetEnum::AMPC: {

			gains.commandedTensionABC.z += increment;
			shared->Display.statusString = "InputClass: Tension C increased to " + shared->FormatDecimal( shared->Controller.commandedTensionABC.z, 2, 2 );

			break;
		}

		// Torque limits
		case selectGainTargetEnum::TORQUE: {

			// Deselect active marker
			shared->Telemetry.activeID = 0;

			// Pick direction
			switch ( shared->Input.selectTorqueTarget ) {

				// Abduction (left)
				case selectTorqueTargetEnum::ABD: {

					// Increment virtual error
					shared->Telemetry.positionFilteredNewMM.x -= increment;

					if ( shared->Controller.commandedPercentageABC.y > shared->Controller.commandedPercentageLimit.y ) {
						shared->Controller.commandedPercentageLimit.y = shared->Controller.commandedPercentageABC.y;
					}

					if ( shared->Controller.commandedPercentageABC.z > shared->Controller.commandedPercentageLimit.z ) {
						shared->Controller.commandedPercentageLimit.z = shared->Controller.commandedPercentageABC.z;
					}

					break;
				}
			}
		}

		// None
		case selectGainTargetEnum::NONE: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				gains.gainKp.abd += increment;
				gains.gainKp.add += increment;
				gains.gainKp.flx += increment;
				gains.gainKp.ext += increment;
				shared->Display.statusString = "InputClass: Kp increased.";

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				gains.gainKi.abd += increment;
				gains.gainKi.add += increment;
				gains.gainKi.flx += increment;
				gains.gainKi.ext += increment;
				shared->Display.statusString = "InputClass: Ki increased.";

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				gains.gainKd.abd += increment;
				gains.gainKd.add += increment;
				gains.gainKd.flx += increment;
				gains.gainKd.ext += increment;
				shared->Display.statusString = "InputClass: Kd increased.";
			}
			break;
		}

		default: {

			// Shouldn't be here...
			break;
		}
	}
}

void InputClass::K_Decrement() {

	// Handler for gains
	auto& gains = shared->Controller;

	// Increment value
	float increment = 0.0f;

	// Select appropriate increment for PID gains
	if ( shared->Input.selectGain == selectGainEnum::KP ) {
		increment = 0.1f;

	} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
		increment = 0.1f;

	} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
		increment = 0.01f;

	} else if ( shared->Input.selectGain == selectGainEnum::NONE ) {
		increment = 0.0f;

	} else {
		increment = 0.0f;
	}

	// Select appropriate increment for tension gains
	if ( shared->Input.selectGainTarget == selectGainTargetEnum::AMPA || shared->Input.selectGainTarget == selectGainTargetEnum::AMPB || shared->Input.selectGainTarget == selectGainTargetEnum::AMPC ) {
		increment = 0.01;
	}

	increment = increment * -1.0f;

	switch ( shared->Input.selectGainTarget ) {

		// Abduction
		case selectGainTargetEnum::ABD: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				SafeDecrement( gains.gainKp.abd, increment );
				shared->Display.statusString = "InputClass: Abduction Kp increased to " + shared->FormatDecimal( shared->Controller.gainKp.abd, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				SafeDecrement( gains.gainKi.abd, increment );
				shared->Display.statusString = "InputClass: Abduction Ki increased to " + shared->FormatDecimal( shared->Controller.gainKi.abd, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				SafeDecrement( gains.gainKd.abd, increment );
				shared->Display.statusString = "InputClass: Abduction Kd increased to " + shared->FormatDecimal( shared->Controller.gainKd.abd, 2, 2 );
			}
			break;
		}

		// Adduction
		case selectGainTargetEnum::ADD: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				SafeDecrement( gains.gainKp.add, increment );
				shared->Display.statusString = "InputClass: Adduction Kp increased to " + shared->FormatDecimal( shared->Controller.gainKp.add, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				SafeDecrement( gains.gainKi.add, increment );
				shared->Display.statusString = "InputClass: Adduction Ki increased to " + shared->FormatDecimal( shared->Controller.gainKi.add, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				SafeDecrement( gains.gainKd.add, increment );
				shared->Display.statusString = "InputClass: Adduction Kd increased to " + shared->FormatDecimal( shared->Controller.gainKd.add, 2, 2 );
			}
			break;
		}

		// Flexion
		case selectGainTargetEnum::FLEX: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				SafeDecrement( gains.gainKp.flx, increment );
				shared->Display.statusString = "InputClass: Flexion Kp increased to " + shared->FormatDecimal( shared->Controller.gainKp.flx, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				SafeDecrement( gains.gainKi.flx, increment );
				shared->Display.statusString = "InputClass: Flexion Ki increased to " + shared->FormatDecimal( shared->Controller.gainKi.flx, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				SafeDecrement( gains.gainKd.flx, increment );
				shared->Display.statusString = "InputClass: Flexion Kd increased to " + shared->FormatDecimal( shared->Controller.gainKd.flx, 2, 2 );
			}
			break;
		}

		// Extension
		case selectGainTargetEnum::EXT: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				SafeDecrement( gains.gainKp.ext, increment );
				shared->Display.statusString = "InputClass: Extension Kp increased to " + shared->FormatDecimal( shared->Controller.gainKp.ext, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				SafeDecrement( gains.gainKi.ext, increment );
				shared->Display.statusString = "InputClass: Extension Ki increased to " + shared->FormatDecimal( shared->Controller.gainKi.ext, 2, 2 );

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				SafeDecrement( gains.gainKd.ext, increment );
				shared->Display.statusString = "InputClass: Extension Kd increased to " + shared->FormatDecimal( shared->Controller.gainKd.ext, 2, 2 );
			}
			break;
		}


		// Amp A
		case selectGainTargetEnum::AMPA: {

			SafeDecrement( gains.commandedTensionABC.x, increment );
			// gains.commandedTensionABC.x += increment;
			shared->Display.statusString = "InputClass: Tension A increased to " + shared->FormatDecimal( shared->Controller.commandedTensionABC.x, 2, 2 );

			break;
		}

		// Amp B
		case selectGainTargetEnum::AMPB: {

			SafeDecrement( gains.commandedTensionABC.y, increment );
			shared->Display.statusString = "InputClass: Tension B increased to " + shared->FormatDecimal( shared->Controller.commandedTensionABC.y, 2, 2 );

			break;
		}

		// Amp C
		case selectGainTargetEnum::AMPC: {

			SafeDecrement( gains.commandedTensionABC.z, increment );
			shared->Display.statusString = "InputClass: Tension C increased to " + shared->FormatDecimal( shared->Controller.commandedTensionABC.z, 2, 2 );

			break;
		}


		// None
		case selectGainTargetEnum::NONE: {

			if ( shared->Input.selectGain == selectGainEnum::KP ) {
				SafeDecrement( gains.gainKp.add, increment );
				SafeDecrement( gains.gainKp.abd, increment );
				SafeDecrement( gains.gainKp.flx, increment );
				SafeDecrement( gains.gainKp.ext, increment );
				shared->Display.statusString = "InputClass: Kp decreased.";

			} else if ( shared->Input.selectGain == selectGainEnum::KI ) {
				SafeDecrement( gains.gainKi.add, increment );
				SafeDecrement( gains.gainKi.abd, increment );
				SafeDecrement( gains.gainKi.flx, increment );
				SafeDecrement( gains.gainKi.ext, increment );
				shared->Display.statusString = "InputClass: Ki decreased.";

			} else if ( shared->Input.selectGain == selectGainEnum::KD ) {
				SafeDecrement( gains.gainKd.add, increment );
				SafeDecrement( gains.gainKd.abd, increment );
				SafeDecrement( gains.gainKd.flx, increment );
				SafeDecrement( gains.gainKd.ext, increment );
				shared->Display.statusString = "InputClass: Kd decreased.";
			}
			break;
		}


		default: {

			// Shouldn't be here...
			break;
		}
	}
}

void InputClass::K_EncoderZero() {

	shared->System.state		 = stateEnum::ZERO_ENCODER;
	shared->Display.statusString = "InputClass: Encoder values set to zero.";
}


void InputClass::K_EncoderMeasureLimit() {


	shared->Amplifier.isMeasuringEncoderLimit = true;
	// shared->Amplifier.isLimitSet			  = false;
	shared->Amplifier.encoderLimitDegA = 0.0f;
	shared->Amplifier.encoderLimitDegB = 0.0f;
	shared->Amplifier.encoderLimitDegC = 0.0f;

	shared->Display.statusString = "InputClass: Measuring Encoder limits.";
}

void InputClass::K_EncoderSetLimit() {

	shared->Amplifier.isLimitSet			  = true;
	shared->Amplifier.isMeasuringEncoderLimit = false;
	shared->Display.statusString			  = "InputClass: Encoder limit set.";
}


void InputClass::K_TaskCalibrationStart() {

	// Update state
	shared->Task.state			 = taskEnum::CALIBRATE;
	shared->Display.statusString = "InputClass: Starting calibration.";
}


void InputClass::K_FittsStart() {
	// Run fitts-law test
	shared->Telemetry.isTargetReset = true;
	shared->Controller.isRampingUp	= true;
	shared->Touchscreen.isTouched	= 0;
	shared->Task.isRunning			= false;
	shared->Task.state				= taskEnum::FITTS;
	shared->Task.repetitionNumber++;
	// shared->TASK_RUNNING = true;
	// shared->fittsTestStarted = false;
	shared->Display.statusString = "InputClass: Starting fitts test.";
}


void InputClass::K_FittsStop() {

	shared->Touchscreen.isTouched = 1;
	shared->Task.state			  = taskEnum::IDLE;
	shared->Display.statusString  = "InputClass: Ending fitts test.";
}



/** OLD
 * 
 */



// void InputClass::ParseInput( int key ) {

// 	// Ignore mouse clicks or arrows
// 	if ( key != 255 ) {

// 		// Uncomment to be able to read detected key presses
// 		std::cout << "InputClass:   Detected key press, index = " << key << "\n";

// 		switch ( key ) {
// 			case 27: {	  // ESC key 1048603
// 				shared->Display.statusString		= "InputClass: Shutting down...";
// 				shared->System.state				= stateEnum::IDLE;
// 				shared->Amplifier.isAmplifierActive = false;
// 				shared->Serial.isSerialSending		= false;
// 				shared->Serial.isSerialSendOpen		= false;
// 				cv::destroyAllWindows();
// 				std::cout << "Shutdown initiated.\n";
// 				shared->System.isShuttingDown = true;
// 				break;
// 			}
// 			case '1': {	   // '1'
// 				shared->Telemetry.activeID	 = 1;
// 				shared->Display.statusString = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 1 ? "1." : "0." );
// 				break;
// 			}
// 			case '2': {	   // '2'
// 				shared->Telemetry.activeID	 = 2;
// 				shared->Display.statusString = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 2 ? "2." : "0." );
// 				break;
// 			}
// 			case '3': {	   // '3'
// 				shared->Telemetry.activeID	 = 3;
// 				shared->Display.statusString = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 3 ? "3." : "0." );
// 				break;
// 			}
// 			case '4': {	   // '4'
// 				shared->Telemetry.activeID	 = 4;
// 				shared->Display.statusString = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 4 ? "4." : "0." );
// 				break;
// 			}
// 			case '5': {	   // '5'
// 				shared->Telemetry.activeID	 = 5;
// 				shared->Display.statusString = "InputClass: Updated active marker to #" + std::string( shared->Telemetry.activeID == 5 ? "5." : "0." );
// 				break;
// 			}
// 			case '`': {	   // ` (tilde)
// 				shared->Display.statusString = "InputClass: Disabling all active markers.";
// 				shared->Telemetry.activeID	 = 0;
// 				break;
// 			}
// 			case 'a': {	   // 'e'
// 				shared->Amplifier.isAmplifierActive = !shared->Amplifier.isAmplifierActive;
// 				if ( shared->Amplifier.isAmplifierActive ) {
// 					shared->Telemetry.isTargetReset	  = true;
// 					shared->Controller.isRampingUp	  = true;
// 					shared->Controller.rampPercentage = 0.0f;
// 					shared->Controller.rampStartTime  = shared->Timing.elapsedRunningTime;
// 					shared->System.state			  = stateEnum::DRIVING_PWM;
// 					shared->Display.statusString	  = "InputClass: Amplifiers enabled.";
// 				} else {
// 					// shared->Serial.packetOut = "DX\n";
// 					// shared->FLAG_PACKET_WAITING = true;
// 					shared->System.state		 = stateEnum::IDLE;
// 					shared->Display.statusString = "InputClass: Amplifiers disabled.";
// 				}
// 				break;
// 			}
// 			case 's': {	   // 's'
// 				// Toggle serial flags
// 				shared->Serial.isSerialSending	 = !shared->Serial.isSerialSending;
// 				shared->Serial.isSerialReceiving = !shared->Serial.isSerialReceiving;
// 				if ( shared->Serial.isSerialSending ) {
// 					shared->Display.statusString		 = "InputClass: Enabling serial IO.";
// 					shared->Serial.isSerialSending		 = true;
// 					shared->Serial.isSerialReceiving	 = true;
// 					shared->Teensy.isAmplifierResponding = false;
// 				} else {
// 					shared->Serial.isSerialSending		 = false;
// 					shared->Serial.isSerialReceiving	 = false;
// 					shared->Teensy.isAmplifierResponding = false;
// 					shared->Display.statusString		 = "InputClass: Disabling serial IO.";
// 				}
// 				break;
// 			}
// 			// case 'x': {	   // 'x'
// 			// 	shared->Display.statusString = "InputClass: Software E-Stop Triggered. Amplifiers and serial output disabled.";
// 			// 	if ( shared->Serial.isSerialSendOpen ) {
// 			// 	}
// 			// 	shared->Amplifier.isAmplifierActive = false;
// 			// 	break;
// 			// }

// 			// Reset gains
// 			case 116: {	   // 'y'
// 				shared->controllerKp		 = cv::Point3f( 0.0f, 0.0f, 0.0f );
// 				shared->controllerKi		 = cv::Point3f( 0.0f, 0.0f, 0.0f );
// 				shared->controllerKd		 = cv::Point3f( 0.0f, 0.0f, 0.0f );
// 				shared->Display.statusString = "InputClass: Setting gains to 0";
// 				break;
// 			}


// 			// Gains X
// 			case 'u': {	   // 'u'
// 				IncrementValueF( "Kp(x)", shared->controllerKp.x, -0.2f );
// 				break;
// 			}
// 			case 'i': {	   // 'i'
// 				IncrementValueF( "Kp(x)", shared->controllerKp.x, 0.2f );
// 				break;
// 			}
// 			case 'o': {	   // 'o'
// 				IncrementValueF( "Ki(x)", shared->controllerKi.x, -0.2f );
// 				break;
// 			}
// 			case 'p': {	   // 'p'
// 				IncrementValueF( "Ki(x)", shared->controllerKi.x, 0.2f );
// 				break;
// 			}
// 			case '[': {	   // '['
// 				IncrementValueF( "Kd(x)", shared->controllerKd.x, -0.01f );
// 				break;
// 			}
// 			case ']': {	   // ']'
// 				IncrementValueF( "Kd(x)", shared->controllerKd.x, 0.01f );
// 				break;
// 			}

// 			// Gains Y
// 			case 'h': {	   // 'h'
// 				IncrementValueF( "Kp(y)", shared->controllerKp.y, -0.2f );
// 				break;
// 			}
// 			case 'j': {	   // 'j'
// 				IncrementValueF( "Kp(y)", shared->controllerKp.y, 0.2f );
// 				break;
// 			}
// 			case 'k': {	   // 'k'
// 				IncrementValueF( "Ki(y)", shared->controllerKi.y, -0.2f );
// 				break;
// 			}
// 			case 'l': {	   // 'l'
// 				IncrementValueF( "Ki(y)", shared->controllerKi.y, 0.2f );
// 				break;
// 			}
// 			case ';': {	   // ';'
// 				IncrementValueF( "Kd(y)", shared->controllerKd.y, -0.01f );
// 				break;
// 			}
// 			case 39: {	  // '''
// 				IncrementValueF( "Kd(y)", shared->controllerKd.y, 0.01f );
// 				break;
// 			}

// 			case 183: {	   // numpad 7
// 				IncrementValueF( "Tension A", shared->Controller.commandedTensionABC.x, 0.01 );
// 				break;
// 			}
// 			case 180: {	   // numpad 4
// 				IncrementValueF( "Tension A", shared->Controller.commandedTensionABC.x, -0.01 );
// 				break;
// 			}
// 			case 184: {	   // numpad 8
// 				IncrementValueF( "Tension B", shared->Controller.commandedTensionABC.y, 0.01 );
// 				break;
// 			}
// 			case 181: {	   // numpad 5
// 				IncrementValueF( "Tension B", shared->Controller.commandedTensionABC.y, -0.01 );
// 				break;
// 			}
// 			case 185: {	   // numpad 9
// 				IncrementValueF( "Tension B", shared->Controller.commandedTensionABC.z, 0.01 );
// 				break;
// 			}
// 			case 182: {	   // numpad 6
// 				IncrementValueF( "Tension B", shared->Controller.commandedTensionABC.z, -0.01 );
// 				break;
// 			}
// 			case 122: {	   // 'z'
// 				shared->vizClear			 = true;
// 				shared->Display.statusString = "InputClass: Resetting visualization trails.";
// 				break;
// 			}
// 			case 99: {	  // 'c'
// 				shared->Task.name			 = "CALIB";
// 				shared->calibrationComplete	 = false;
// 				shared->Display.statusString = "InputClass: Starting calibration.";
// 				break;
// 			}
// 			case 118: {	   // 'v'
// 				shared->vizEnabled			 = !shared->vizEnabled;
// 				shared->Display.statusString = "InputClass: 3D Visualization " + std::string( shared->vizEnabled ? "enabled." : "disabled." );
// 				break;
// 			}
// 			// case 116: {	   // 't'
// 			// 	shared->angleEnabled  = !shared->angleEnabled;
// 			// 	shared->displayString = "InputClass: Angle Visualization " + std::string( shared->angleEnabled ? "enabled." : "disabled." );
// 			// 	break;
// 			// }
// 			case 102: {	   // 'f'
// 				// Run fitts-law test
// 				shared->Telemetry.isTargetReset = true;
// 				shared->Controller.isRampingUp	= true;
// 				shared->Touchscreen.isTouched	= 0;
// 				shared->Task.isRunning			= false;
// 				shared->Task.name				= "FITTS";
// 				shared->Task.repetitionNumber++;
// 				// shared->TASK_RUNNING = true;
// 				// shared->fittsTestStarted = false;
// 				shared->Display.statusString = "InputClass: Starting fitts-law test.";
// 				break;
// 			}
// 			case 103: {	   // 'g'
// 				// End test
// 				shared->Touchscreen.isTouched = 1;
// 				// shared->TASK_RUNNING = false;
// 				// shared->TASK_NAME	 = "FITTS_ANGLE";
// 				// shared->angleEnabled = true;
// 				// shared->TASK_RUNNING = true;
// 				// shared->fittsTestStarted = false;
// 				shared->Display.statusString = "InputClass: Starting angle test.";
// 				break;
// 			}
// 			case 98: {	  // 'b'
// 				shared->Touchscreen.isTouched = 1;
// 				shared->Display.statusString  = "InputClass: Ending angle test.";
// 				break;
// 			}
// 			case 113: {	   // 'q'
// 				shared->System.state		 = stateEnum::MEASURING_LIMITS;
// 				shared->Display.statusString = "InputClass: Encoder reset.";
// 				break;
// 			}
// 			default: {
// 				shared->Display.statusString = "InputClass: Unknown input.";
// 				break;
// 			}
// 		}
// 	} else {
// 	}
// }