#include <Bounce.h>

// -----------------------------------------------------------------------------
// Hardware Mapping

static const byte sPins[8] = 
{
    0, 1, 2, 3, 4, 5, 6, 7
};

static const byte sPinMode = 14; // A0
static const byte sPinIncr = 15; // A1
static const byte sPinDecr = 16; // A2

static const unsigned long sDebouncingTime = 10; // ms
Bounce sModeButton(sPinMode, sDebouncingTime);
Bounce sIncrButton(sPinIncr, sDebouncingTime);
Bounce sDecrButton(sPinDecr, sDebouncingTime);

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
byte sCurrentStep = 0;

// -----------------------------------------------------------------------------

bool yield(unsigned inTime)
{
    static const unsigned sSleepTime = 20;
    const unsigned numIterations = max(1, inTime / sSleepTime);

    for (unsigned i = 0; i < numIterations; ++i)
    {
        sModeButton.update();
        if (sModeButton.fallingEdge())
        {
            sRunningMode = !sRunningMode;
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
        digitalWrite(sPins[i], LOW);
    }
}

// -----------------------------------------------------------------------------

void runAuto()
{
    while (true)
    {
        const unsigned current = sCurrentStep;
        sCurrentStep = (sCurrentStep + 1) & 0x7;
        digitalWrite(sPins[current],        LOW);
        digitalWrite(sPins[sCurrentStep],   HIGH);
        if (yield(1000))
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
        const unsigned current = sCurrentStep;
        sIncrButton.update();
        sDecrButton.update();

        if (sIncrButton.fallingEdge())
        {
            sCurrentStep = (sCurrentStep + 1) & 0x7;
            digitalWrite(sPins[current],        LOW);
            digitalWrite(sPins[sCurrentStep],   HIGH);
        }
        else if (sDecrButton.fallingEdge())
        {
            sCurrentStep = (sCurrentStep - 1) & 0x7;
            digitalWrite(sPins[current],        LOW);
            digitalWrite(sPins[sCurrentStep],   HIGH);
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
    pinMode(sPinMode, INPUT_PULLUP);
    pinMode(sPinIncr, INPUT_PULLUP);
    pinMode(sPinDecr, INPUT_PULLUP);

    for (byte i = 0; i < 8; ++i)
    {
        pinMode(sPins[i], OUTPUT);
    }
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

