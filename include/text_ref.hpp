#ifndef TEXT_REF_HPP_INCLUDED
#define TEXT_REF_HPP_INCLUDED

#include <string>

class PrimitiveText;

namespace salmon {
    class TextRef {
        friend class MapRef;
        public:
            TextRef(PrimitiveText& impl);
            TextRef(PrimitiveText* impl);

            bool good() const {return (m_impl == nullptr) ? false : true ;}

            struct Attributes {
                std::string font_family = "sans-serif";
                struct {
                    unsigned char r = 0;
                    unsigned char g = 0;
                    unsigned char b = 0;
                    unsigned char a = 255;
                } color;
                unsigned pixel_size = 12;
                bool wrap = false, bold = false, italic = false, underline = false, strikeout = false, kerning = true;
            };

            bool get_static_mode() const;
            void set_static_mode(bool mode);

            bool get_hidden() const;
            void set_hidden(bool mode);

            std::string get_text();
            Attributes get_attributes();
            int get_w();
            int get_h();

            void set_text(std::string text);
            void set_attributes(Attributes a);

            bool apply();

        private:
            PrimitiveText* m_impl;
    };
}


#endif // TEXT_REF_HPP_INCLUDED
