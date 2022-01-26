#include <iostream>
#include <string>
#include <vector>
#include <MidiFile.h> // midifile.sapp.org || github.com/craigsapp/midifile

using namespace std;

class TransformNote
{
 public:
    TransformNote(int t=0, float _a=1., int _b=0) : track(t), a(_a), b(_b) {}
    int track;
    float a;
    int b;
};
typedef vector<TransformNote> vtfn_t;

class MidiConv
{
 public:
    MidiConv(
        smf::MidiFile& _mf,
        const vtfn_t& tnfv,
        bool _verbose=false
    ) :
        mf(_mf),
        trans_notes_velocity(tnfv),
        verbose(_verbose),
        pxform_note_default(nullptr)
    {
        pxform_note_default = pxform_note_of_track_index(-1);
    }
    int run();
 private:
    const TransformNote* pxform_note_of_track(int track) const
    {
        const TransformNote *ret = pxform_note_of_track_index(track);
        ret = ret ? : pxform_note_default;
        return ret;
    }
    bool verbose;
    const TransformNote* pxform_note_of_track_index(int track) const;
    void xform_event_velocity(const TransformNote& x, smf::MidiEvent& e);
    smf::MidiFile& mf;
    vtfn_t trans_notes_velocity;
    const TransformNote *pxform_note_default;
};

int MidiConv::run()
{
    int rc = 0;
    int n_tracks = mf.size();
    cerr << "#(tracks) = " << n_tracks << '\n';
    for (int ti = 0; ti < n_tracks; ++ti)
    {
        smf::MidiEventList& events = mf[ti];
        int n_events = events.size();
        int n_notes_on = 0;
        const TransformNote *px = pxform_note_of_track(ti);
        cerr << "track[" << ti << "] #(events) = " << n_events << '\n';
        for (int ei = 0; ei < n_events; ++ei)
        {
            smf::MidiEvent& e = events[ei];
            if (e.isMetaMessage() && e.isTrackName())
            {
                cout << "trackname(" << ti << "): " << e.getMetaContent() <<
                     '\n';
            }
            if (e.isNoteOn())
            {
                ++n_notes_on;
                if (verbose)
                {
                    cout << ti << "/" << ei << ": NoteOn" <<
                        ", key: " << e.getKeyNumber() <<
                        ", velocity: " << e.getVelocity() << 
                        ", tick-" << e.tick <<
                        ", duration-" << e.getDurationInSeconds() << '\n';
                }
                if (px)
                {
                    xform_event_velocity(*px, e);
                }
            }
        }
    }
    return rc;
}

const TransformNote* MidiConv::pxform_note_of_track_index(int track) const
{
    const TransformNote *ret = nullptr;
    for (const TransformNote& xform_note: trans_notes_velocity)
    {
        if (xform_note.track == track)
        {
            ret = &xform_note;
        }
    }
    return ret;
}

void MidiConv::xform_event_velocity(const TransformNote& x, smf::MidiEvent& e)
{
    int v = e.getVelocity();
    v = static_cast<int>(x.a * v) + x.b;
    if (v < 0) { v = 0; }
    if (v > 0x7f) { v = 0x7f; }
    e.setVelocity(v);
}

class UMidiConv
{
 public:
    UMidiConv(int argc, char **argv);
    int run();
    int get_rc() const { return rc; }
 private:
    void usage(const char *p0) const;
    int rc;
    const char* fn_in;
    const char* fn_out;
    vtfn_t trans_notes_velocity;
};

UMidiConv::UMidiConv(int argc, char **argv) :
    rc(0),
    fn_in(nullptr),
    fn_out(nullptr)
{
    int ai = 1;
    for ( ; (rc == 0) && (ai < argc) && (argv[ai][0] == '-'); )
    {
         const string opt(argv[ai++]);
         if (opt == string("-tv"))
         {
             int t = (ai < argc ? strtoul(argv[ai++], 0, 0) : -1);
             float a = (ai < argc ? strtof(argv[ai++], 0) : -1);
             int b = (ai < argc ? strtoul(argv[ai++], 0, 0) : -1);
             if (ai < argc)
             {
                 trans_notes_velocity.push_back(TransformNote(t, a, b));
             }
             else
             {
                 cerr << "missing -tv values\n";
                 rc = 1;
             }
         }
         else
         {
             cerr << "Bad optopn: " << opt << '\n';
             rc = 1;
         }
    }
    if ((rc == 0) && (ai +2 != argc))
    {
        cerr << "Missing file name(s)\n";
        rc = 1;
    }
    else
    {
        fn_in = argv[ai++];
        fn_out = argv[ai++];
    }
    if (rc != 0)
    {
        usage(argv[0]);
    }
}

void UMidiConv::usage(const char *p0) const
{
    cerr << "Usage: \n" <<
        p0 << '\n' << 
        "  [-tv <n> <a> <b>]  # Change velocity for track <n>, V := aV + b\n"
        "                     # (n = -1 all other tracks) Repeatable\n";
        "  <in midi> <out midi>\n";
}

int UMidiConv::run()
{
    smf::MidiFile mf;
    if (!mf.read(string(fn_in)))
    {
        cerr << "Failed to read " << fn_in << '\n';
        rc = 1;
    }
    if (rc == 0)
    {
        MidiConv mc(mf, trans_notes_velocity);
        rc = mc.run();
    }
    if (rc == 0)
    {
        if (!mf.write(string(fn_out)))
        {
            cerr << "Failed to write " << fn_out << '\n';
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    UMidiConv p(argc, argv);
    int rc = (p.get_rc() ? : p.run());
    return rc;
}

#if 0 // old
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
    smf::MidiFile mf;
    mf.read(string(fn));
    midiconv(mf);
    return rc;
}
#endif // old
