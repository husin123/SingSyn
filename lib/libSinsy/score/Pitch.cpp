#include <algorithm>
#include <stdexcept>

#include "Pitch.h"
#include "util_string.h"
#include "util_log.h"

namespace sinsy {

    const Pitch::Step Pitch::C = 0;
    const Pitch::Step Pitch::Db = 1;
    const Pitch::Step Pitch::D = 2;
    const Pitch::Step Pitch::Eb = 3;
    const Pitch::Step Pitch::E = 4;
    const Pitch::Step Pitch::F = 5;
    const Pitch::Step Pitch::Gb = 6;
    const Pitch::Step Pitch::G = 7;
    const Pitch::Step Pitch::Ab = 8;
    const Pitch::Step Pitch::A = 9;
    const Pitch::Step Pitch::Bb = 10;
    const Pitch::Step Pitch::B = 11;

    const Pitch::Step Pitch::DEFAULT_STEP = 0;
    const Pitch::Octave Pitch::DEFAULT_OCTAVE = 4;

    namespace {
        const size_t STEP_NUM = 12;
        const std::string STEPS[STEP_NUM] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};

        class CompPitch {
        public:
            //! constructor
            CompPitch(const std::string &str) : target(str) {}

            //! destructor
            virtual ~CompPitch() {}

            //! ...
            bool operator()(const std::string &str) const {
                return (0 == target.compare(str)) ? true : false;
            }

            //! target
            const std::string &target;
        };

    };

/*!
 constructor
 */
    Pitch::Pitch() : step(DEFAULT_STEP), octave(DEFAULT_OCTAVE) {
    }

/*!
 constructor
 */
    Pitch::Pitch(Step s, Octave o, int a) {
        set(s, o, a);
    }

/*!
 constructor
 */
    Pitch::Pitch(const std::string &s, Octave o, int a) {
        set(s, o, a);
    }

/*!
 copy constructor
 */
    Pitch::Pitch(const Pitch &obj) : step(obj.step), octave(obj.octave) {
    }

/*!
 destructor
 */
    Pitch::~Pitch() {
    }

/*!
 assignment operator
 */
    Pitch &Pitch::operator=(const Pitch &obj) {
        if (this != &obj) {
            this->step = obj.step;
            this->octave = obj.octave;
        }
        return *this;
    }

/*!
 equrl
 */
    bool Pitch::operator==(const Pitch &obj) const {
        if (this == &obj) {
            return true;
        }
        return ((step == obj.step) && (octave == obj.octave)) ? true : false;
    }

/*!
 not equrl
 */
    bool Pitch::operator!=(const Pitch &obj) const {
        return !(*this == obj);
    }

/*!
 set
 */
    void Pitch::set(Pitch::Step s, Pitch::Octave o, int alter) {
        s += alter;
        while (s < 0) {
            s += 12;
            --o;
        }
        while (11 < s) {
            s -= 12;
            ++o;
        }
        this->step = s;
        this->octave = o;
    }

/*!
 set
 */
    void Pitch::set(const std::string &s, Pitch::Octave o, int alter) {
        std::string str(s);
        toUpper(str);
        const std::string *itr(std::find_if(STEPS, STEPS + STEP_NUM, CompPitch(str)));
        if (itr < STEPS + STEP_NUM) {
            set(itr - STEPS, o, alter);
            return;
        }
        ERR_MSG("Unexpected pitch : " << str << " [step:" << s << ", octave:" << o << ", alter:" << alter << "]");
        throw std::runtime_error("Pitch::set() invalid argument");
    }

/*!
 get step
 */
    Pitch::Step Pitch::getStep() const {
        return step;
    }

/*!
 get octave
 */
    Pitch::Octave Pitch::getOctave() const {
        return octave;
    }

/*!
 get string of step
 */
    const std::string &Pitch::getStepStr() const {
        return STEPS[this->step];
    }


/*!
 addition
 */
    Pitch &Pitch::operator+=(int i) {
        step += i;
        while (step < 0) {
            step += 12;
            if (0 == octave) {
                std::range_error("Pitch octave is out of range : octave < 0");
            }
            --octave;
        }
        while (11 < step) {
            step -= 12;
            ++octave;
        }
        return *this;
    }

/*!
 subtraction
 */
    Pitch &Pitch::operator-=(int i) {
        return (*this) += (-i);
    }

/*!
 increment
 */
    Pitch &Pitch::operator++() {
        return (*this) += 1;
    }

/*!
 decrement
 */
    Pitch &Pitch::operator--() {
        return (*this) -= 1;
    }

/*!
 addition
 */
    Pitch operator+(const Pitch &pitch, int i) {
        Pitch ret(pitch);
        return ret += i;
    }

/*!
 subtraction
 */
    Pitch operator-(const Pitch &pitch, int i) {
        return pitch + (-i);
    }

/*!
 increment
 */
    Pitch operator++(const Pitch &pitch) {
        Pitch ret(pitch);
        return pitch + 1;
    }

/*!
 decrement
 */
    Pitch operator--(const Pitch &pitch) {
        Pitch ret(pitch);
        return pitch - 1;
    }


/*!
 to stream
 */
    std::ostream &operator<<(std::ostream &os, const Pitch &pitch) {
        return os << pitch.getStepStr() << pitch.getOctave();
    }

};  // namespace sinsy
