#include "text_ref.hpp"

#include "actor/primitive_text.hpp"

using salmon::TextRef;

TextRef::TextRef(PrimitiveText& impl) : m_impl{&impl} {}
TextRef::TextRef(PrimitiveText* impl) : m_impl{impl} {}

std::string TextRef::get_text() {return m_impl->get_text();}
TextRef::Attributes TextRef::get_attributes() {
    PrimitiveText::Attributes a = m_impl->get_attributes();
    return {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
}
int TextRef::get_w() {return m_impl->get_w();}
int TextRef::get_h() {return m_impl->get_h();}

void TextRef::set_text(std::string text) {m_impl->set_text(text);}
void TextRef::set_attributes(Attributes a) {
    PrimitiveText::Attributes temp = {a.font_family, {a.color.r,a.color.g,a.color.b,a.color.a}, a.pixel_size, a.wrap, a.bold, a.italic, a.underline, a.strikeout, a.kerning};
    m_impl->set_attributes(temp);
}

bool TextRef::apply() {return m_impl->generate_texture();}

bool TextRef::get_static_mode() const {return m_impl->get_static_mode();}
void TextRef::set_static_mode(bool mode) {m_impl->set_static_mode(mode);}

bool TextRef::get_hidden() const {return m_impl->get_hidden();}
void TextRef::set_hidden(bool mode) {m_impl->set_hidden(mode);}
