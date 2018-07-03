#include "SimpAC.h"
#include <cstring>
#include <cstdio>

const char * css_test1 = ""


"A,B,C:c(a,s),D{ font-family: \"Microsoft YaHei\"; }"

"#btn0 { font-family: Microsoft YaHei; }"
"#btn1 { image: url('https://a.b.c/d.png') 30 }"

"\0"
" .css * :hover , #ele {/*asd*/"
" color: red   ; "
"}"
;

const char * css_test = u8R"(

button#button, #layout .button{
    -moz-appearance: none;
    transition: 0.5s;
    background-color: red;
    background-color: blue;
}

#layout button:hover {
    background-color: yellow "sad";


    color: rgb(255,20,0) ;


    background-image: url('../images/2 5.png') ;

    font: italic bold .8em/ 1.2 Arial;
}

.imgButton {
    -moz-appearance: none;
    border-width: 3px;
    border-image-source: url(../images/btn.png);
    border-image-slice: 3 fill;
}
)"
;

using namespace SimpAC;

struct Css final : SimpAC::CACStream {
    virtual void add_comment(StrPair) noexcept override;
    virtual void add_selector(BasicSelectors, StrPair) noexcept override;
    virtual void add_selector_combinator(Combinators) noexcept override;
    virtual void add_selector_comma() noexcept override;
    virtual void begin_properties() noexcept override;
    virtual void end_properties() noexcept override;
    virtual void begin_property(StrPair) noexcept override;
    virtual void add_value(StrPair) noexcept override;
    virtual void add_func_value(FuncValue, StrPair) noexcept override;
    virtual void add_attribute_selector(BasicSelectors, StrPair, StrPair) noexcept override;
    static void printf(StrPair);
};


int main() {
    Css parser;
    parser.Load({ css_test, css_test + std::strlen(css_test) });
    //std::printf("\nPRESS 'O' to OUTPUT FUNC-LIST\n");
    const char ch = std::getchar();
    //if (ch == 'O' || ch == 'o') {

    //    std::getchar();
    //}
    return 0;
}

void Css::printf(StrPair view) {
    const int len = view.second - view.first;
    std::printf("%.*s", len, view.first);
}


void Css::add_comment(StrPair view) noexcept {
    std::printf("add_comment: ");
    this->printf(view);
    std::putchar('\n');
}

#ifdef SAC_ATTRIBUTE_SELECTOR
void Css::add_attribute_selector(BasicSelectors sel, StrPair a, StrPair b) noexcept {
    auto type = "";
    switch (sel)
    {
    case Selectors_AttributeSet:
        type = "[x]";
        b.second = b.first;
        break;
    case Selectors_AttributeExact:
        type = "[x=y]";
        break;
    case Selectors_AttributeList:
        type = "[x~=y]";
        break;
    case Selectors_AttributeHyphen:
        type = "[x|=y]";
        break;
    case Selectors_AttributeContain:
        type = "[x*=y]";
        break;
    case Selectors_AttributeBegin:
        type = "[x^=y]";
        break;
    case Selectors_AttributeEnd:
        type = "[x&=y]";
        break;
    }

    std::printf("add_selector: ");
    this->printf(a);
    std::printf(type);
    this->printf(b);
    std::putchar('\n');

}
#endif 

void Css::add_selector(BasicSelectors sel, StrPair view) noexcept {
    auto t = "";
    switch (sel)
    {
    case Selectors_Type:
        t = "<element>";
        break;
    case Selectors_Class:
        t = "<class>";
        break;
    case Selectors_Id:
        t = "<id>";
        break;
    case Selectors_Universal:
        t = "<*>";
        break;
    case Selectors_PseudoClass:
        t = "<p-class>";
        break;
    case Selectors_PseudoElement:
        t = "<p-element>";
        break;
    }

    std::printf("add_selector: %s", t);
    this->printf(view);
    std::putchar('\n');

}

void Css::add_selector_combinator(Combinators c) noexcept {
    const char* str = "";
    switch (c)
    {
    case Combinators_AdjacentSibling:
        str = "A + B";
        break;
    case Combinators_GeneralSibling:
        str = "A ~ B";
        break;
    case Combinators_Child:
        str = "A > B";
        break;
    case Combinators_Descendant:
        str = "A   B";
        break;
    }

    std::printf("add_selector_combinator: %s\n", str);
}

void Css::add_selector_comma() noexcept {
    std::printf("add_selector_comma\n");
}

void Css::begin_properties() noexcept {
    std::printf("begin_properties {\n");
}

void Css::end_properties() noexcept {
    std::printf("} end_properties\n\n");
}

void Css::begin_property(StrPair view) noexcept {
    std::printf("  begin_propertie: ");
    this->printf(view);
    std::putchar('\n');
}

void Css::add_value(StrPair view) noexcept {
    std::printf("  --add_value: ");
    this->printf(view);
    std::putchar('\n');
}

void Css::add_func_value(FuncValue value, StrPair raw_func) noexcept {
    std::printf("  --add_value: ");
    this->printf(raw_func);
    std::putchar(':');
    this->printf(StrPair{ value.first, value.first + value.length });
    std::putchar('\n');
} // 