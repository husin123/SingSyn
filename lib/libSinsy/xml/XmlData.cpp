#include <algorithm>
#include "XmlData.h"
#include "Deleter.h"
#include "util_log.h"
#include "util_string.h"

namespace sinsy {

    namespace {
        class StringOutputter {
        public:
            //! constructor
            StringOutputter(std::ostream &s) : stream(s) {}

            //! copy constructor
            StringOutputter(const StringOutputter &obj) : stream(obj.stream) {}

            //! destructor
            virtual ~StringOutputter() {}

            //! ...
            void operator()(const IStringWritable *obj) {
                stream << *obj;
            }

        private:
            //! assignment operator (donot use)
            StringOutputter &operator=(const StringOutputter &);

            //! stream
            std::ostream &stream;
        };

        class AttributeOutputter {
        public:
            //! constructor
            AttributeOutputter(std::ostream &s) : stream(s) {}

            //! copy constructor
            AttributeOutputter(const AttributeOutputter &obj) : stream(obj.stream) {}

            //! destructor
            virtual ~AttributeOutputter() {}

            //! ...
            void operator()(const std::pair<std::string, std::string> &obj) {
                stream << " " << obj.first << "=\"" << obj.second << "\"";
            }

        private:
            //! assignment operator (donot use)
            AttributeOutputter &operator=(const AttributeOutputter &);

            //! stream
            std::ostream &stream;
        };

    }; // namespace

/*!
 constructor

 @param t tag
 @param d data
 */
    XmlData::XmlData(const std::string &t, const std::string &d) : tag(t), data(d) {
        toLower(tag);
    }

/*!
 copy constructor
 */
    XmlData::XmlData(const XmlData &obj) : tag(obj.tag), data(obj.data), attributes(obj.attributes) {
        try {
            // copy children
            children.reserve(obj.children.size());
            const Children::const_iterator itrEnd(obj.childEnd());
            for (Children::const_iterator itr(obj.childBegin()); itrEnd != itr; ++itr) {
                this->addChild(new XmlData(**itr));
            }
        } catch (const std::exception &) {
            // delete children
            std::for_each(children.begin(), children.end(), Deleter<XmlData>());
            throw;
        }
    }

/*!
 destructor
 */
    XmlData::~XmlData() {
        std::for_each(children.begin(), children.end(), Deleter<XmlData>());
    }

/*!
 get tag
 */
    const std::string &XmlData::getTag() const {
        return tag;
    }

/*!
 get data
 */
    const std::string &XmlData::getData() const {
        return data;
    }

/*!
 set data
 */
    void XmlData::setData(const std::string &str) {
        data = str;
    }

/*!
 add child
 */
    XmlData::Children::iterator XmlData::addChild(XmlData *child) {
        if (NULL == child) {
            std::runtime_error("XmlData::addChild() NULL pointer");
        }
        children.push_back(child);
        return children.end() - 1;
    }

//! erase child
    XmlData::Children::iterator XmlData::eraseChild(const Children::iterator &itr) {
        delete *itr;
        return children.erase(itr);
    }

/*!
 add attribute
 */
    bool XmlData::addAttribute(const std::string &k, const std::string &v) {
        std::string key(k);
        std::string value(v);
        toLower(key);
        toLower(value);
        if (!attributes.insert(std::make_pair(key, value)).second) {
            ERR_MSG("Failed to insert attribute (" << key << ", " << value << ")");
            return false;
        }
        return true;
    }

/*!
 get attribute
 */
    const std::string &XmlData::getAttribute(const std::string &k) const {
        Attributes::const_iterator itr = attributes.find(k);
        if (attributes.end() == itr) {
            return NULL_STR;
        }
        return itr->second;
    }

/*!
 to string stream
 */
    std::ostream &XmlData::toStringStream(std::ostream &stream) const {
        stream << "<" << tag;
        std::for_each(attributes.begin(), attributes.end(), AttributeOutputter(stream));
        if (data.empty() && children.empty()) {
            stream << " />\n";
            return stream;
        }
        stream << ">" << data;
        if (!children.empty()) {
            stream << "\n";
            std::for_each(children.begin(), children.end(), StringOutputter(stream));
        }
        stream << "</" << tag << ">\n";
        return stream;
    }

/*!
 get begin const iterator of children
 */
    XmlData::Children::const_iterator XmlData::childBegin() const {
        return children.begin();
    }

/*!
 get end const iterator of children
 */
    XmlData::Children::const_iterator XmlData::childEnd() const {
        return children.end();
    }

/*!
 get begin iterator of children
 */
    XmlData::Children::iterator XmlData::childBegin() {
        return children.begin();
    }

/*!
 get end iterator of children
 */
    XmlData::Children::iterator XmlData::childEnd() {
        return children.end();
    }

};  // namespace sinsy
