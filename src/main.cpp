#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <unordered_map>
#include "ratings_data.hpp"
#include "sprites_data.hpp"
using namespace geode::prelude;

static std::unordered_map<std::string, CCTexture2D*> s_diffTextures;

$execute {
    struct Entry { const char* name; const unsigned char* data; int size; };
    static const Entry entries[] = {
        {"ez",          k_spr_ez,           k_spr_ez_size          },
        {"med",         k_spr_med,          k_spr_med_size         },
        {"hard",        k_spr_hard,         k_spr_hard_size        },
        {"difftochall", k_spr_difftochall,  k_spr_difftochall_size },
        {"challtoint",  k_spr_challtoint,   k_spr_challtoint_size  },
        {"inttorem",    k_spr_inttorem,     k_spr_inttorem_size    },
        {"remtoins",    k_spr_remtoins,     k_spr_remtoins_size    },
        {"instoext",    k_spr_instoext,     k_spr_instoext_size    },
        {"terri",       k_spr_terri,        k_spr_terri_size       },
        {"cata",        k_spr_cata,         k_spr_cata_size        },
        {"horri",       k_spr_horri,        k_spr_horri_size       },
        {"unreal",      k_spr_unreal,       k_spr_unreal_size      },
        {"nil",         k_spr_nil,          k_spr_nil_size         },
    };
    for (auto const& e : entries) {
        auto img = new CCImage();
        if (img->initWithImageData((void*)e.data, e.size)) {
            auto tex = new CCTexture2D();
            if (tex->initWithImage(img)) {
                tex->retain();
                s_diffTextures[e.name] = tex;
            } else {
                tex->release();
            }
        }
        img->release();
    }
}

static std::string getSubRating(float rating) {
    int tier = (int)rating;
    int dec  = (int)std::round((rating - tier) * 100.f);

    const char* tierName = "";
    switch (tier) {
        case 10: tierName = "Terrifying";   break;
        case 11: tierName = "Catastrophic"; break;
        case 12: tierName = "Horrific";     break;
        case 13: tierName = "Unreal";       break;
        case 14: tierName = "Nil";          break;
        default: return "";
    }

    const char* sub = "";
    if      (dec == 0)  sub = "Baseline";
    else if (dec <= 9)  sub = "Bottom";
    else if (dec <= 24) sub = "Bottom-Low";
    else if (dec <= 33) sub = "Low";
    else if (dec <= 44) sub = "Low-Mid";
    else if (dec <= 59) sub = "Mid";
    else if (dec <= 69) sub = "Mid-High";
    else if (dec <= 79) sub = "High";
    else if (dec <= 94) sub = "High-Peak";
    else                sub = "Peak";

    return std::string(sub) + " " + tierName;
}

static std::string strLower(std::string s) {
    for (auto& c : s) c = (char)std::tolower((unsigned char)c);
    return s;
}

static ccColor3B getDifficultyColor(std::string_view name) {
    if (name == "Easy")         return {0x76, 0xF4, 0x47};
    if (name == "Medium")       return {0xFF, 0xFF, 0x00};
    if (name == "Hard")         return {0xFE, 0x7C, 0x00};
    if (name == "Difficult")    return {0xFF, 0x32, 0x32};
    if (name == "Challenging")  return {0xA0, 0x00, 0x00};
    if (name == "Intense")      return {0x19, 0x22, 0x2D};
    if (name == "Remorseless")  return {0xC9, 0x00, 0xC8};
    if (name == "Insane")       return {0x00, 0x00, 0xFF};
    if (name == "Extreme")      return {0x02, 0x8A, 0xFF};
    if (name == "Terrifying")   return {0x00, 0xFF, 0xFF};
    if (name == "Catastrophic") return {0xFF, 0xFF, 0xFF};
    if (name == "Horrific")     return {0x96, 0x91, 0xFF};
    if (name == "Unreal")       return {0x4B, 0x00, 0xC8};
    if (name == "Nil")          return {0x65, 0x66, 0x6D};
    return {0xFF, 0xFF, 0xFF};
}

struct SubSegment {
    std::string text;
    bool        isColored;
    ccColor3B   color;
};

// Splits subText into colored (difficulty name) and uncolored (modifier) segments.
// Format: "[Modifier] DiffName [ - [Modifier] DiffName]"
// The last word of each " - "-separated part is always the difficulty name.
static std::vector<SubSegment> parseSubText(std::string const& subText) {
    std::vector<SubSegment> result;
    size_t pos = 0;
    bool firstPart = true;

    while (pos <= subText.size()) {
        size_t sep  = subText.find(" - ", pos);
        size_t end  = (sep == std::string::npos) ? subText.size() : sep;
        std::string part = subText.substr(pos, end - pos);

        if (!firstPart)
            result.push_back({" - ", false, {0xFF, 0xFF, 0xFF}});
        firstPart = false;

        size_t lastSpace = part.rfind(' ');
        if (lastSpace == std::string::npos) {
            result.push_back({part, true, getDifficultyColor(part)});
        } else {
            result.push_back({part.substr(0, lastSpace + 1), false, {0xFF, 0xFF, 0xFF}});
            std::string diffName = part.substr(lastSpace + 1);
            result.push_back({diffName, true, getDifficultyColor(diffName)});
        }

        if (sep == std::string::npos) break;
        pos = sep + 3;
    }

    return result;
}

static std::optional<float> findRating(std::string const& levelName, std::string const& creatorName) {
    auto lname    = strLower(levelName);
    auto lcreator = strLower(creatorName);

    RatingEntry const* noUserMatch = nullptr;
    RatingEntry const* userMatch   = nullptr;
    bool anyMatch = false;

    for (auto const& e : k_ratings) {
        if (strLower(std::string(e.base)) != lname) continue;
        anyMatch = true;
        if (e.user.empty() && !noUserMatch)
            noUserMatch = &e;
        else if (!e.user.empty() && strLower(std::string(e.user)) == lcreator)
            userMatch = &e;
    }

    if (!anyMatch)   return std::nullopt;
    if (userMatch)   return userMatch->rating;
    if (noUserMatch) return noUserMatch->rating;
    return std::nullopt;
}

class $modify(MyLevelInfoLayer, LevelInfoLayer) {
    struct Fields {
        CCLabelBMFont* m_rangeLabel = nullptr;
        CCLabelBMFont* m_nameLabel  = nullptr;
    };

    void addStackedRange(CCPoint pos, std::string const& num1, ccColor3B col1, std::string const& num2, ccColor3B col2, float xOffset = 0.f) {
        float scale   = 0.43f;
        float x       = pos.x - 77.f + xOffset;
        float yMid    = pos.y - 26.f;
        float spacing = 9.f;

        auto lbl1 = CCLabelBMFont::create(num1.c_str(), "bigFont.fnt");
        lbl1->setScale(scale);
        lbl1->setColor(col1);
        lbl1->setAnchorPoint({0.5f, 0.5f});
        lbl1->setPosition({x, yMid + spacing});
        this->addChild(lbl1, 999);

        auto dash = CCLabelBMFont::create("-", "bigFont.fnt");
        dash->setScale(scale * 0.7f);
        dash->setAnchorPoint({0.5f, 0.5f});
        dash->setPosition({x, yMid});
        this->addChild(dash, 999);

        auto lbl2 = CCLabelBMFont::create(num2.c_str(), "bigFont.fnt");
        lbl2->setScale(scale);
        lbl2->setColor(col2);
        lbl2->setAnchorPoint({0.5f, 0.5f});
        lbl2->setPosition({x, yMid - spacing});
        this->addChild(lbl2, 999);

        m_fields->m_rangeLabel = lbl1;
    }

    void addLabels(CCPoint pos, std::string const& rangeText, std::string const& subText, float subScale, bool rightAlign = false, float rangeLabelXOffset = 0.f, float subTextYOffset = 0.f, float rangeLabelScale = 0.45f, float rangeLabelYOffset = 0.f) {
        auto rangeLabel = CCLabelBMFont::create(rangeText.c_str(), "bigFont.fnt");
        rangeLabel->setScale(rangeLabelScale);
        if (rightAlign)
            rangeLabel->setAnchorPoint({1.f, 0.5f});
        rangeLabel->setPosition({pos.x - 77.f + rangeLabelXOffset, pos.y - 21.f + rangeLabelYOffset});
        this->addChild(rangeLabel, 999);
        m_fields->m_rangeLabel = rangeLabel;

        if (!subText.empty()) {
            float scale   = subScale * 0.8f;
            float yPos    = pos.y - 36.f + subTextYOffset;
            float xRight  = pos.x - 77.f + rangeLabelXOffset;

            auto segments = parseSubText(subText);

            std::vector<CCNode*> nodes;
            float totalWidth = 0.f;

            for (auto& seg : segments) {
                auto lbl = CCLabelBMFont::create(seg.text.c_str(), "bigFont.fnt");
                lbl->setScale(scale);
                if (seg.isColored)
                    lbl->setColor(seg.color);
                totalWidth += lbl->getContentSize().width * scale;
                nodes.push_back(lbl);
            }

            float xOffset = rightAlign ? (xRight - totalWidth) : (xRight - totalWidth / 2.f);
            CCLabelBMFont* firstLabel = nullptr;
            for (auto node : nodes) {
                float w = node->getContentSize().width * node->getScale();
                node->setAnchorPoint({0.f, 0.5f});
                node->setPosition({xOffset, yPos});
                this->addChild(node, 999);
                xOffset += w;
                if (!firstLabel)
                    firstLabel = dynamic_cast<CCLabelBMFont*>(node);
            }

            m_fields->m_nameLabel = firstLabel;
        }
    }

    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        std::string levelName   = level->m_levelName;
        std::string creatorName = level->m_creatorName;

        // Extract creator from "LevelName (CreatorName)" format
        if (!levelName.empty() && levelName.back() == ')') {
            auto ppos = levelName.rfind('(');
            if (ppos != std::string::npos && ppos > 0) {
                creatorName = levelName.substr(ppos + 1, levelName.size() - ppos - 2);
                levelName   = levelName.substr(0, ppos);
                while (!levelName.empty() && levelName.back() == ' ')
                    levelName.pop_back();
            }
        }

        std::string rangeText  = "";
        std::string subText    = "";
        float       subScale   = 0.45f;
        std::string iconKey    = "";
        bool        rightAlign = false;
        bool        useStacked = false;
        std::string stackNum1  = "", stackNum2  = "";
        ccColor3B   stackCol1  = {0xFF,0xFF,0xFF}, stackCol2 = {0xFF,0xFF,0xFF};

        int  stars     = level->m_stars;
        bool demon     = level->m_demon;
        int  demonDiff = level->m_demonDifficulty;

        if (!demon && !level->m_autoLevel) {
            if      (stars == 2)               { rangeText = "1-1.99";    subText = "Easy";                                 subScale = 0.45f; iconKey = "ez";          rightAlign = true; }
            else if (stars == 3)               { rangeText = "2-2.99";    subText = "Medium";                               subScale = 0.45f; iconKey = "med";         rightAlign = true; }
            else if (stars == 4 || stars == 5) { rangeText = "3-3.5";     subText = "Baseline Hard - Mid Hard";             subScale = 0.26f; iconKey = "hard";        }
            else if (stars == 6 || stars == 7) { rangeText = "3.5-3.99";  subText = "Mid Hard - Peak Hard";                 subScale = 0.28f; iconKey = "hard";        }
            else if (stars == 8 || stars == 9) {
                iconKey = "difftochall"; useStacked = true;
                stackNum1 = "4";    stackCol1 = getDifficultyColor("Difficult");
                stackNum2 = "5.5";  stackCol2 = getDifficultyColor("Challenging");
            }
        } else if (demon) {
            if (demonDiff == 3) {
                iconKey = "challtoint"; useStacked = true;
                stackNum1 = "5.5";  stackCol1 = getDifficultyColor("Challenging");
                stackNum2 = "6.5";  stackCol2 = getDifficultyColor("Intense");
            } else if (demonDiff == 4) {
                iconKey = "inttorem"; useStacked = true;
                stackNum1 = "6.5";   stackCol1 = getDifficultyColor("Intense");
                stackNum2 = "7.25";  stackCol2 = getDifficultyColor("Remorseless");
            } else if (demonDiff == 0) {
                iconKey = "remtoins"; useStacked = true;
                stackNum1 = "7.25";  stackCol1 = getDifficultyColor("Remorseless");
                stackNum2 = "8.75";  stackCol2 = getDifficultyColor("Insane");
            } else if (demonDiff == 5) {
                iconKey = "instoext"; useStacked = true;
                stackNum1 = "8.75";  stackCol1 = getDifficultyColor("Insane");
                stackNum2 = "9.99";  stackCol2 = getDifficultyColor("Extreme");
            }
            else {
                auto ratingOpt = findRating(levelName, creatorName);
                if (ratingOpt.has_value()) {
                    float rating = ratingOpt.value();
                    rangeText  = fmt::format("{:.2f}", rating);
                    subText    = getSubRating(rating);
                    subScale   = 0.24f;
                    rightAlign = true;
                    int tier = (int)rating;
                    if      (tier == 10) iconKey = "terri";
                    else if (tier == 11) iconKey = "cata";
                    else if (tier == 12) iconKey = "horri";
                    else if (tier == 13) iconKey = "unreal";
                    else if (tier == 14) iconKey = "nil";
                } else {
                    rangeText = "N/A";
                    subScale  = 0.45f;
                }
            }
        }

        if (rangeText.empty() && !useStacked) return true;

        GJDifficultySprite* diffSprite = nullptr;
        for (auto child : CCArrayExt<CCNode*>(this->getChildren())) {
            if (auto ds = typeinfo_cast<GJDifficultySprite*>(child)) {
                diffSprite = ds;
                break;
            }
        }
        if (!diffSprite) return true;

        bool  isExtremeDemon    = demon && demonDiff != 3 && demonDiff != 4 && demonDiff != 0 && demonDiff != 5;
        bool  isEasyOrNormal    = !demon && (stars == 2 || stars == 3);
        bool  applySpecial      = isExtremeDemon || isEasyOrNormal;
        float rangeLabelXOffset = applySpecial ? 14.f   : 0.f;
        float subTextYOffset    = applySpecial ? 1.f    : 0.f;
        float rangeLabelScale   = applySpecial ? 0.525f : 0.45f;
        float rangeLabelYOffset = applySpecial ? -1.f   : 0.f;

        if (useStacked) {
            addStackedRange(diffSprite->getPosition(), stackNum1, stackCol1, stackNum2, stackCol2);
        } else {
            addLabels(diffSprite->getPosition(), rangeText, subText, subScale, rightAlign, rangeLabelXOffset, subTextYOffset, rangeLabelScale, rangeLabelYOffset);
        }

        // Add category icon next to the difficulty face
        if (!iconKey.empty()) {
            auto it = s_diffTextures.find(iconKey);
            if (it != s_diffTextures.end()) {
                auto iconSpr = CCSprite::createWithTexture(it->second);
                if (iconSpr) {
                    CCSize diffSize  = diffSprite->getContentSize();
                    float  diffScale = diffSprite->getScale();
                    float  targetH   = diffSize.height * diffScale;
                    bool   isEzMed   = (iconKey == "ez" || iconKey == "med" || iconKey == "difftochall");
                    float  sprScale  = (targetH / iconSpr->getContentSize().height) * (isEzMed ? 0.7f : 0.425f);
                    iconSpr->setScale(sprScale);

                    CCPoint diffPos = diffSprite->getPosition();
                    float xPos = diffPos.x - (diffSize.width * diffScale) / 2.f
                                           - (iconSpr->getContentSize().width * sprScale) / 2.f
                                           - 13.0f
                                           + (iconKey == "med" ? 3.f : 0.f);
                    iconSpr->setPosition({xPos, diffPos.y - 27.f});
                    this->addChild(iconSpr, 999);
                }
            }
        }

        return true;
    }
};
