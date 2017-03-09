#include <Bounce.h>

// -----------------------------------------------------------------------------
// Hardware Mapping - Arduino Leonardo

#define ENABLE_SERIAL 0

template<int Pin>
class ToggleButton : public Bounce
{
public:
    static const int sDebouncingTimeMs = 10;

public:
    inline ToggleButton()
        : Bounce(Pin, sDebouncingTimeMs)
        , mState(false)
    {

    }

public:
    static inline void init()
    {
        pinMode(Pin, INPUT_PULLUP);
    }

    inline bool process()
    {
        const bool status = update();

        if (fallingEdge())
        {
            mState = true; // Pressed
        }
        else if (risingEdge())
        {
            mState = false; // Released
        }
        if (mState != mPreviousState)
        {
            mPreviousState = mState;
            return mState;
        }
        return false;
    }
private:
    bool mState;
    bool mPreviousState;
};

static const byte sOutputPins[8] =
{
    0, 1, 2, 3, 4, 5, 6, 7
};

static const byte sModePin = 20; // A2
static const byte sDecrPin = 21; // A3
static const byte sIncrPin = 22; // A4
static const byte sGndPin  = 23; // A5
static const byte sLedPin  = 13; // LED

ToggleButton<sModePin> sModeButton;
ToggleButton<sIncrPin> sIncrButton;
ToggleButton<sDecrPin> sDecrButton;

// -----------------------------------------------------------------------------

struct RunningMode
{
    enum
    {
        automatic,
        manual,

        numModes,
    };
};

byte sRunningMode = RunningMode::automatic;
byte sCurrentStep = 7;

// -----------------------------------------------------------------------------

bool yield(unsigned inTime)
{
    static const unsigned sSleepTime = 20;
    const unsigned numIterations = max(1, inTime / sSleepTime);

    for (unsigned i = 0; i < numIterations; ++i)
    {
        if (sModeButton.process())
        {
            sRunningMode = !sRunningMode;
#if ENABLE_SERIAL
            Serial.print("Running Mode: ");
            Serial.println(sRunningMode ? "manual" : "automatic");
#endif
            return true;
        }
        delay(sSleepTime);
    }
    return false;
}

// -----------------------------------------------------------------------------

void reset()
{
    for (byte i = 0; i < 8; ++i)
    {
        digitalWrite(sOutputPins[i], LOW);
    }
}

void updatePins(byte inPrevious)
{
    digitalWrite(sOutputPins[inPrevious],   LOW);
    digitalWrite(sOutputPins[sCurrentStep], HIGH);
#if ENABLE_SERIAL
    Serial.print("Pin ");
    Serial.println(sCurrentStep + 1);
#else
    digitalWrite(sLedPin, HIGH);
    delay(50);
    digitalWrite(sLedPin, LOW);
#endif
}

// -----------------------------------------------------------------------------

void runAuto()
{
    while (true)
    {
        const unsigned previous = sCurrentStep;
        sCurrentStep = (sCurrentStep + 1) & 0x7;
        updatePins(previous);
        if (yield(500))
        {
            reset();
            break;
        }
    }
}

void runManual()
{
    while (true)
    {
        const unsigned previous = sCurrentStep;
        const bool incr = sIncrButton.process();
        const bool decr = sDecrButton.process();

        if (incr)
        {
            sCurrentStep = (sCurrentStep + 1) & 0x7;
            updatePins(previous);
        }
        else if (decr)
        {
            sCurrentStep = (sCurrentStep - 1) & 0x7;
            updatePins(previous);
        }

        if (yield(0))
        {
            reset();
            break;
        }
    }
}

// -----------------------------------------------------------------------------

void setup()
{
    sModeButton.init();
    sIncrButton.init();
    sDecrButton.init();
    pinMode(sGndPin, OUTPUT); // GND reference

    for (byte i = 0; i < 8; ++i)
    {
        pinMode(sOutputPins[i], OUTPUT);
    }
#if ENABLE_SERIAL
    Serial.begin(115200);
    while (!Serial);
    Serial.print("Ready. Running mode: ");
    Serial.println(sRunningMode ? "manual" : "automatic");
#else
    pinMode(sLedPin, OUTPUT);
#endif
}

void loop()
{
    switch (sRunningMode)
    {
        case RunningMode::automatic:
            runAuto();
            break;
        case RunningMode::manual:
            runManual();
        default:
            break;
    }
}

