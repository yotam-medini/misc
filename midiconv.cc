#include <iostream>
#include <string>
#include <MidiFile.h> // midifile.sapp.org || github.com/craigsapp/midifile

using namespace std;

int midiconv(smf::MidiFile& mf)
{
    int rc = 0;
    int n_tracks = mf.size();
    cout << "n_tracks = " << n_tracks << '\n';
    for (int ti = 0; ti < n_tracks; ++ti)
    {
        smf::MidiEventList& events = mf[ti];
        int n_events = events.size();
        cout << "track[" << ti << "] n_events = " << n_events << '\n';
        for (int ei = 0; ei < n_events; ++ei)
        {
            smf::MidiEvent& e = events[ei];
            if (e.isMetaMessage())
            {
                string meta = e.getMetaContent();
                cout << ti << "/" << ei << 
                    " meta: type; " << e.getMetaType() <<
                    ", isTrackName: " << e.isTrackName() << 
                    ", isInstrumentName: " << e.isInstrumentName() << 
                    ": content: " << meta << '\n';
            }
            if (e.isNoteOn() || e.isNoteOff())
            {
                const char *onoff = (e.isNoteOn() ? "On " : "Off");
                cout << ti << "/" << ei << ": Note" << onoff <<
                    ", key: " << e.getKeyNumber() <<
                    ", velocity: " << e.getVelocity() << 
                    ", tick-" << e.tick <<
                    ", duration-" << e.getDurationInSeconds() << '\n';
            }
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    const char* fn = argv[1];
    // smf::MidiFile mf(string(fn));
    smf::MidiFile mf;
    mf.read(string(fn));
    midiconv(mf);
    return rc;
}
