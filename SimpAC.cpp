#include "SimpAC.h"
#include <cassert>



// simpcs::impl namepsace
namespace SimpAC { namespace impl {
    // table for valid selector
    static const uint32_t valid_selector_table[] = {
        0x00000000, 0x03ff2000, 0x87fffffe, 0x07fffffe,
    };
#if 0
    // table maker
    void make_valid_selector_table() noexcept {
        auto valid = [](char ch) noexcept {
            return ch == '-' || ch == '_'
                || (ch >= '0' && ch <= '9')
                || (ch >= 'a' && ch <= 'z')
                || (ch >= 'A' && ch <= 'Z')
                ;
        };
        const int char_bit = CHAR_BIT;
        const int len = 128 / char_bit * char_bit;
        const int ary = len / char_bit / sizeof(uint32_t);
        uint32_t buffer[ary];
        std::memset(buffer, 0, sizeof buffer);
        for (int i = 0; i <= len; ++i) {
            const int index = i >> 5;
            const int offset = i & 31;
            buffer[index] |= valid(i) << offset;
        }
        if (const auto file = std::fopen("out.txt", "w")) {
            for (auto x : buffer) {
                std::fprintf(file, "0x%08x, ", x);
            }
            std::fclose(file);
            std::exit(0);
        }
    }
#endif
    // is space?
    static inline bool is_space(char ch) noexcept {
        return (ch == ' ') || (ch == '\t');
    }
    // is quot attr?
    static inline bool is_quot(char ch) noexcept {
        return (ch == '"') || (ch == '\'');
    }
    // is number start, TODO: remove ->
    static auto is_valid_selector(char ch) noexcept -> uint32_t {
        return valid_selector_table[ch >> 5] & uint32_t(1 << (ch & 31));
    }
}}

/// <summary>
/// state for combinator
/// </summary>
enum class SimpAC::CACStream::combinator_state {
    reset, has, ready, release
};

/// <summary>
/// state for css
/// </summary>
enum class SimpAC::CACStream::css_state : unsigned {
    standby = 0, adjacent, general,
    child, pseudo, id, class_,
    universal, selectors,
    properties, properties_end,
    values, values_quot, values_end, atrule,
#ifdef SAC_ATTRIBUTE_SELECTOR
    attribute,
    attribute_type, attribute_value_begin,
    attribute_value_end, attribute_end,
#endif
};


#ifndef SAC_PURE_VIRTUAL
/// <summary>
/// Adds the comment.
/// </summary>
/// <param name="comment">The comment.</param>
/// <returns></returns>
void SimpAC::CACStream::add_comment(StrPair comment) noexcept {
}

/// <summary>
/// Adds the selector.
/// </summary>
/// <param name="type">The type.</param>
/// <param name="name">The name.</param>
/// <returns></returns>
void SimpAC::CACStream::add_selector(BasicSelectors type, StrPair name) noexcept {
}

/// <summary>
/// Adds the selector combinator.
/// </summary>
/// <param name="combinator">The combinator.</param>
/// <returns></returns>
void SimpAC::CACStream::add_selector_combinator(Combinators combinator) noexcept {
}

/// <summary>
/// Adds the selector comma.
/// </summary>
/// <returns></returns>
void SimpAC::CACStream::add_selector_comma() noexcept {
}

/// <summary>
/// Begins the properties.
/// </summary>
/// <returns></returns>
void SimpAC::CACStream::begin_properties() noexcept {
}

/// <summary>
/// Ends the properties.
/// </summary>
/// <returns></returns>
void SimpAC::CACStream::end_properties() noexcept {
}

/// <summary>
/// Begins the property.
/// </summary>
/// <param name="property">The property.</param>
/// <returns></returns>
void SimpAC::CACStream::begin_property(StrPair property) noexcept {
}

/// <summary>
/// Adds the value.
/// </summary>
/// <param name="value">The value.</param>
/// <returns></returns>
void SimpAC::CACStream::add_value(StrPair value) noexcept {
}



#ifdef SAC_ATTRIBUTE_SELECTOR
/// <summary>
/// Adds the attribute selector.
/// </summary>
/// <param name="type">The type.</param>
/// <param name="attribute">The attribute.</param>
/// <param name="value">The value.</param>
/// <returns></returns>
void SimpAC::CACStream::add_attribute_selector(
    BasicSelectors type, StrPair attribute, StrPair value) noexcept {

}
#endif

#endif

/// <summary>
/// Parses the comment.
/// </summary>
/// <param name="view">The view.</param>
/// <returns></returns>
bool SimpAC::CACStream::parse_comment(StrPair & view) noexcept {
    // TODO: ����һ�� && �жϷŵ�while������ٴ�����
    // /*ע��
    if (view.first + 1 < view.second && view.first[1] == '*') {
        view.first += 2;
        auto comment_view = view;
        bool comment = false;
        // ����ֱ���ַ���������������*/
        while (view.first < view.second) {
            if (comment) {
                if (view.first[0] == '/') break;
                comment = false;
            }
            else if (view.first[0] == '*') comment = true;
            ++view.first;
        }
        // ȥ��һ��*
        comment_view.second = view.first - 1;
        this->add_comment(comment_view);
        ++view.first;
        return true;
    }
    return false;
}



/// <summary>
/// Parses the selector LV1.
/// </summary>
/// <param name="ch">The ch.</param>
/// <param name="state">The state.</param>
/// <returns></returns>
auto SimpAC::CACStream::parse_selector_lv1(char ch, combinator_state& state) noexcept ->css_state {
    switch (ch)
    {
    default: return css_state::selectors;
    case '@': return css_state::atrule;
    case '.': return css_state::class_;
    case '#': return css_state::id;
    case '*': return css_state::universal;
    case ':': return css_state::pseudo;
#ifdef SAC_ATTRIBUTE_SELECTOR
    case '[': return css_state::attribute;
#elif !defined(NDEBUG)
    case '[': assert(!"unsupport"); return css_state::selectors;
#endif
    case '+': return css_state::adjacent;
    case '~': return css_state::general;
    case '>': return css_state::child;
    case '{':
        this->begin_properties();
        return css_state::properties;
    case ',':
        state = combinator_state::reset;
        this->add_selector_comma();
        return css_state::standby;
    case ' ': case '\t':
        if (state == combinator_state::has)
            state = combinator_state::ready;
        return css_state::standby;
    }
}



/// <summary>
/// Loads the specified string.
/// </summary>
/// <param name="view">The view.</param>
/// <returns></returns>
void SimpAC::CACStream::Load(StrPair view) noexcept {
    // ״̬
    BasicSelectors selector;
    //Combinators combinator;
    StrPair this_view, ex_view;
    char last_quot = 0;
    auto state = css_state::standby;
    auto show_combinator = combinator_state::reset;
    // ����״̬
    while (view.first < view.second) {
        // ���ȴ���ע��
        if (view.first[0] == '/' && this->parse_comment(view))
            continue;
        // ��ǰ�����ַ�
        const auto ch = view.first[0];
        // ״̬����
        switch (state)
        {
        case css_state::standby:
            // Ĭ�ϵ�Ԫ��ѡ����
            selector = BasicSelectors::Selectors_Type;
            this_view.first = view.first + 1;
            state = this->parse_selector_lv1(ch, show_combinator);
            // �����ѵ����ַ�
            if (state == css_state::selectors) this_view.first--;
            break;
        case css_state::adjacent:
            // A + B
            this->add_selector_combinator(Combinators_AdjacentSibling);
            goto add_combinator;
        case css_state::general:
            // A ~ B
            this->add_selector_combinator(Combinators_General);
            goto add_combinator;
        case css_state::child:
            // A + B
            this->add_selector_combinator(Combinators_Child);
        add_combinator:
            state = css_state::standby;
            show_combinator = combinator_state::release;
            continue;
        case css_state::pseudo:
            // αԪ��ѡ����
            if (ch == ':') {
                this_view.first = view.first + 1;
                selector = BasicSelectors::Selectors_PseudoElement;
            }
            // α��ѡ����
            else selector = BasicSelectors::Selectors_PseudoClass;
            state = css_state::selectors;
            break;
        case css_state::id:
            // idѡ����
            selector = BasicSelectors::Selectors_Id;
            state = css_state::selectors;
            break;
        case css_state::class_:
            // ��ѡ����
            selector = BasicSelectors::Selectors_Class;
            state = css_state::selectors;
            break;
        case css_state::universal:
            // ͨ��ѡ����
            selector = BasicSelectors::Selectors_Universal;
            state = css_state::selectors;
            continue;
        case css_state::selectors:
            // ����ϵѡ����״̬
            if (show_combinator == combinator_state::ready) {
                this->add_selector_combinator(Combinators_Descendant);
            }
            show_combinator = combinator_state::has;
            // ����?
            if (ch == '(') {
                while (*view.first != ')') ++view.first;
                ++view.first;
                assert(view.first <= view.second && "out of string");
                // TODO: ע��֧��
                goto invalid_selector;
            }
            // ��Чѡ�����ַ� -> ���ѡ����
            else if (!impl::is_valid_selector(ch)) {
            invalid_selector:
                this_view.second = view.first;
                this->add_selector(selector, this_view);
                state = css_state::standby;
                continue;
            }
            break;
        case css_state::properties:
            // ����������
            if (ch == '}') goto end_of_properties;
            // ���Ե����� ;
            else if (ch == ';');
            // �����ǿո�
            else if (!impl::is_space(ch)) {
                this_view.first = view.first;
                state = css_state::properties_end;
            }
            break;
        case css_state::properties_end:
            // ���ҿո����:
            if (ch == ':' || impl::is_space(ch)) {
                this_view.second = view.first;
                this->begin_property(this_view);
                // ֱ������ð��
                while (*view.first != ':') ++view.first;
                assert(view.first <= view.second && "out of string");
                // TODO: ע��֧��

                // �л���ֵ״̬
                state = css_state::values;
            }
            break;
        case css_state::values:
            // ��������
            if (ch == ';') state = css_state::properties;
            // ֱ�ӽ���
            else if (ch == '}') goto end_of_properties;
            // Ѱ��ֵ��ʼ��
            else if (!impl::is_space(ch)) {
                // ð��?
                if (impl::is_quot(ch)) {
                    last_quot = ch;
                    this_view.first = view.first + 1;
                    state = css_state::values_quot;
                }
                // ������?
                else /*if (ch == '(') {
                     assert(!"NOT IMPL");
                     }
                     else*/ {
                    this_view.first = view.first;
                    state = css_state::values_end;
                }
            }
            break;
        case css_state::values_quot:
            // Ѱ��ֵ������
            if (ch == last_quot) {
                this_view.second = view.first;
                this->add_value(this_view);
                state = css_state::values;
            }
            break;
        case css_state::values_end:
            // Ѱ��ֵ������
            if (impl::is_space(ch) || ch == ';') {
                this_view.second = view.first;
                this->add_value(this_view);
                state = ch == ';'
                    ? css_state::properties
                    : css_state::values;
            }
            break;
        end_of_properties:
            this->end_properties();
            state = css_state::standby;
            show_combinator = combinator_state::reset;
            break;
        case css_state::atrule:
            break;
#ifdef SAC_ATTRIBUTE_SELECTOR
        case css_state::attribute:
            // ����ѡ����, �׶�1, Ѱ�Ҽ�
            if (!impl::is_space(ch)) {
                this_view.first = view.first;
                selector = BasicSelectors::Selectors_AttributeSet;
                state = css_state::attribute_type;
            }
        case css_state::attribute_type:
            // ����ѡ����, �׶�3, Ѱ������ѡ��������
        {
            switch (ch)
            {
            default:
                this_view.second = view.first + 1;
                break;
            case ' ': case '\t':
                break;
            case ']':
                // ֱ�ӽ���
                goto end_of_attribute;
            case '=':
                selector = BasicSelectors::Selectors_AttributeExact;
                goto after_state;
            case '~':
                selector = BasicSelectors::Selectors_AttributeList;
                goto after_eq;
            case '|':
                selector = BasicSelectors::Selectors_AttributeHyphen;
                goto after_eq;
            case '*':
                selector = BasicSelectors::Selectors_AttributeContain;
                goto after_eq;
            case '^':
                selector = BasicSelectors::Selectors_AttributeBegin;
                goto after_eq;
            case '&':
                selector = BasicSelectors::Selectors_AttributeEnd;
            after_eq:
                assert(view.first[1] == '=');
                view.first++;
            after_state:
                state = css_state::attribute_value_begin;
                break;
            }
        }
        break;
        case css_state::attribute_value_begin:
            // ����ѡ����, �׶�4, Ѱ��ֵ��ʼ
            if (!impl::is_space(ch)) {
                ex_view.first = impl::is_quot(ch) ? view.first + 1 : view.first;
                state = css_state::attribute_value_end;
            }
            break;
        case css_state::attribute_value_end:
            // ����ѡ����, �׶�5, Ѱ��ֵ����
            if (impl::is_quot(ch) || impl::is_space(ch)) {
                ex_view.second = view.first;
                state = css_state::attribute_end;
            }
            break;
        case css_state::attribute_end:
            // ����ѡ����, �׶�6, Ѱ�����Խ���
            if (ch == ']') {
            end_of_attribute:
                this->add_attribute_selector(selector, this_view, ex_view);
                state = css_state::standby;
            }
            break;
#endif
        }
        // �ƽ�
        ++view.first;
    }
}
