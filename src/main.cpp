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
        {"Easy",         k_spr_easy,     k_spr_easy_size    },
        {"Medium",       k_spr_medium,   k_spr_medium_size  },
        {"Hard",         k_spr_hard,     k_spr_hard_size    },
        {"Difficult",    k_spr_diff,     k_spr_diff_size    },
        {"Challenging",  k_spr_chall,    k_spr_chall_size   },
        {"Intense",      k_spr_intense,  k_spr_intense_size },
        {"Remorseless",  k_spr_rem,      k_spr_rem_size     },
        {"Insane",       k_spr_insane,   k_spr_insane_size  },
        {"Extreme",      k_spr_extreme,  k_spr_extreme_size },
        {"Terrifying",   k_spr_terri,    k_spr_terri_size   },
        {"Catastrophic", k_spr_cata,     k_spr_cata_size    },
        {"Horrific",     k_spr_horri,    k_spr_horri_size   },
        {"Unreal",       k_spr_unreal,   k_spr_unreal_size  },
        {"Nil",          k_spr_nil,      k_spr_nil_size     },
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
    if      (dec == 0)            sub = "Baseline";
    else if (dec <= 9)            sub = "Bottom";
    else if (dec <= 24)           sub = "Bottom-Low";
    else if (dec <= 33)           sub = "Low";
    else if (dec <= 44)           sub = "Low-Mid";
    else if (dec <= 59)           sub = "Mid";
    else if (dec <= 69)           sub = "Mid-High";
    else if (dec <= 79)           sub = "High";
    else if (dec <= 94)           sub = "High-Peak";
    else                          sub = "Peak";

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

    void addLabels(CCPoint pos, std::string const& rangeText, std::string const& subText, float subScale) {
        auto rangeLabel = CCLabelBMFont::create(rangeText.c_str(), "bigFont.fnt");
        rangeLabel->setScale(0.45f);
        rangeLabel->setPosition({pos.x - 77.f, pos.y - 21.f});
        this->addChild(rangeLabel, 999);
        m_fields->m_rangeLabel = rangeLabel;

        if (!subText.empty()) {
            float scale   = subScale * 0.8f;
            float yPos    = pos.y - 36.f;
            float xCenter = pos.x - 77.f;

            auto segments = parseSubText(subText);

            // Build node list: sprite + label for colored segments, label only for uncolored
            std::vector<CCNode*> nodes;
            float totalWidth = 0.f;
            float sprTargetH = 57.f * scale; // scale sprite to match bigFont character height

            for (auto& seg : segments) {
                if (seg.isColored) {
                    auto it = s_diffTextures.find(seg.text);
                    if (it != s_diffTextures.end()) {
                        auto spr = CCSprite::createWithTexture(it->second);
                        if (spr) {
                            bool isEasyMed = (seg.text == "Easy" || seg.text == "Medium");
                            float sizeMultiplier = isEasyMed ? 0.75f : 1.0f;
                            float sprScale = (sprTargetH * sizeMultiplier) / spr->getContentSize().height;
                            spr->setScale(sprScale);
                            totalWidth += spr->getContentSize().width * sprScale;
                            nodes.push_back(spr);
                        }
                    }
                }
                auto lbl = CCLabelBMFont::create(seg.text.c_str(), "bigFont.fnt");
                lbl->setScale(scale);
                if (seg.isColored)
                    lbl->setColor(seg.color);
                totalWidth += lbl->getContentSize().width * scale;
                nodes.push_back(lbl);
            }

            // Place all nodes left-to-right, centered around xCenter
            float xOffset = xCenter - totalWidth / 2.f;
            CCLabelBMFont* firstLabel = nullptr;
            for (auto node : nodes) {
                float w = node->getContentSize().width * node->getScale();
                bool isSprite = (dynamic_cast<CCLabelBMFont*>(node) == nullptr);
                node->setAnchorPoint({0.f, 0.5f});
                node->setPosition({xOffset + (isSprite ? -2.f : 0.f), yPos});
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

        std::string rangeText = "";
        std::string subText   = "";
        float       subScale  = 0.45f;

        int  stars     = level->m_stars;
        bool demon     = level->m_demon;
        int  demonDiff = level->m_demonDifficulty;

        if (!demon && !level->m_autoLevel) {
            if      (stars == 2)               { rangeText = "1-1.99";    subText = "Easy";                                 subScale = 0.45f; }
            else if (stars == 3)               { rangeText = "2-2.99";    subText = "Medium";                               subScale = 0.45f; }
            else if (stars == 4 || stars == 5) { rangeText = "3-3.5";     subText = "Baseline Hard - Mid Hard";             subScale = 0.26f; }
            else if (stars == 6 || stars == 7) { rangeText = "3.5-3.99";  subText = "Mid Hard - Peak Hard";                 subScale = 0.28f; }
            else if (stars == 8 || stars == 9) { rangeText = "4-5.5";     subText = "Baseline Difficult - Mid Challenging"; subScale = 0.19f; }
        } else if (demon) {
            if      (demonDiff == 3) { rangeText = "5.5-6.5";   subText = "Mid Challenging - Mid Intense"; subScale = 0.24f; }
            else if (demonDiff == 4) { rangeText = "6.5-7.25";  subText = "Mid Intense - Low Remorseless"; subScale = 0.23f; }
            else if (demonDiff == 0) { rangeText = "7.25-8.75"; subText = "Low Remorseless - High Insane"; subScale = 0.23f; }
            else if (demonDiff == 5) { rangeText = "8.75-9.99"; subText = "High Insane - Peak Extreme";    subScale = 0.22f; }
            else {
                std::string levelName   = level->m_levelName;
                std::string creatorName = level->m_creatorName;

                if (!levelName.empty() && levelName.back() == ')') {
                    auto pos = levelName.rfind('(');
                    if (pos != std::string::npos && pos > 0) {
                        creatorName = levelName.substr(pos + 1, levelName.size() - pos - 2);
                        levelName   = levelName.substr(0, pos);
                        while (!levelName.empty() && levelName.back() == ' ')
                            levelName.pop_back();
                    }
                }

                auto ratingOpt = findRating(levelName, creatorName);
                if (ratingOpt.has_value()) {
                    rangeText = fmt::format("{:.2f}", ratingOpt.value());
                    subText   = getSubRating(ratingOpt.value());
                    subScale  = 0.28f;
                } else {
                    rangeText = "N/A";
                    subScale  = 0.45f;
                }
            }
        }

        if (rangeText.empty()) return true;

        GJDifficultySprite* diffSprite = nullptr;
        for (auto child : CCArrayExt<CCNode*>(this->getChildren())) {
            if (auto ds = typeinfo_cast<GJDifficultySprite*>(child)) {
                diffSprite = ds;
                break;
            }
        }
        if (!diffSprite) return true;

        addLabels(diffSprite->getPosition(), rangeText, subText, subScale);
        return true;
    }
};
