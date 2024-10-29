#ifndef REMO_H
#define REMO_H
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    const char *str;
    const char *description;
} remo_t;

remo_t remo[] = {
    {"\U0001F600", "Grinning Face"},                   // 😀
    {"\U0001F601", "Beaming Face with Smiling Eyes"},  // 😁
    {"\U0001F602", "Face with Tears of Joy"},          // 😂
    {"\U0001F923", "Rolling on the Floor Laughing"},   // 🤣
    {"\U0001F603", "Grinning Face with Big Eyes"},     // 😃
    {"\U0001F604", "Grinning Face with Smiling Eyes"}, // 😄
    {"\U0001F609", "Winking Face"},                    // 😉
    {"\U0001F60A", "Smiling Face with Smiling Eyes"},  // 😊
    {"\U0001F60D", "Smiling Face with Heart-Eyes"},    // 😍
    {"\U0001F618", "Face Blowing a Kiss"},             // 😘
    {"\U0001F617", "Kissing Face"},                    // 😗
    {"\U0001F61A", "Kissing Face with Closed Eyes"},   // 😚
    {"\U0001F642", "Slightly Smiling Face"},           // 🙂
    {"\U0001F643", "Upside-Down Face"},                // 🙃
    {"\U0001F970", "Smiling Face with Hearts"},        // 🥰
    {"\U0001F60B", "Face Savoring Food"},              // 😋
    {"\U0001F61B", "Face with Tongue"},                // 😛
    {"\U0001F61C", "Winking Face with Tongue"},        // 😜
    {"\U0001F92A", "Zany Face"},                       // 🤪
    {"\U0001F929", "Star-Struck"},                     // 🤩
    {"\U0001F631", "Face Screaming in Fear"},          // 😱
    {"\U0001F62D", "Loudly Crying Face"},              // 😭
    {"\U0001F624", "Face with Steam From Nose"},       // 😤
    {"\U0001F620", "Angry Face"},                      // 😠
    {"\U0001F621", "Pouting Face"},                    // 😡
    {"\U0001F47B", "Ghost"},                           // 👻
    {"\U0001F480", "Skull"},                           // 💀
    {"\U0001F4A9", "Pile of Poo"},                     // 💩
    {"\U0001F47D", "Alien"},                           // 👽
                                                       // Geometric Shapes
    {"\U000025A0", "Black Square"},                    // ■
    {"\U000025B2", "Upward Triangle"},                 // ▲
    {"\U000025CF", "Black Circle"},                    // ●
    {"\U000025CB", "White Circle"},                    // ○
    {"\U00002B1B", "Large Black Square"},              // ⬛
    {"\U00002B1C", "Large White Square"},              // ⬜

    // Mathematical Symbols
    {"\U00002200", "For All"},       // ∀
    {"\U00002203", "Exists"},        // ∃
    {"\U00002205", "Empty Set"},     // ∅
    {"\U00002207", "Nabla"},         // ∇
    {"\U0000220F", "N-Ary Product"}, // ∏
    {"\U00002212", "Minus Sign"},    // −
    {"\U0000221E", "Infinity"},      // ∞

    // Arrows
    {"\U00002190", "Left Arrow"},        // ←
    {"\U00002191", "Up Arrow"},          // ↑
    {"\U00002192", "Right Arrow"},       // →
    {"\U00002193", "Down Arrow"},        // ↓
    {"\U00002195", "Up Down Arrow"},     // ↕
    {"\U00002197", "Up Right Arrow"},    // ↗
    {"\U00002198", "Down Right Arrow"},  // ↘
    {"\U000027A1", "Black Right Arrow"}, // ➡️

    // Dingbats
    {"\U00002714", "Check Mark"},             // ✔️
    {"\U00002716", "Heavy Multiplication X"}, // ✖️
    {"\U00002728", "Sparkles"},               // ✨
    {"\U00002757", "Exclamation Mark"},       // ❗
    {"\U0000274C", "Cross Mark"},             // ❌
    {"\U00002795", "Heavy Plus Sign"},        // ➕

    // Miscellaneous Symbols
    {"\U00002600", "Sun"},                      // ☀️
    {"\U00002614", "Umbrella with Rain Drops"}, // ☔
    {"\U00002620", "Skull and Crossbones"},     // ☠️
    {"\U000026A0", "Warning Sign"},             // ⚠️
    {"\U000026BD", "Soccer Ball"},              // ⚽
    {"\U000026C4", "Snowman"},                  // ⛄

    // Stars and Asterisks
    {"\U00002733", "Eight Pointed Black Star"}, // ✳️
    {"\U00002734", "Eight Spoked Asterisk"},    // ✴️
    {"\U00002B50", "White Star"},               // ⭐
    {"\U0001F31F", "Glowing Star"},             // 🌟
    {"\U00002728", "Sparkles"},                 // ✨
                                                // Animals and Nature
    {"\U0001F98A", "Fox"},                      // 🦊
    {"\U0001F415", "Dog"},                      // 🐕
    {"\U0001F431", "Cat Face"},                 // 🐱
    {"\U0001F435", "Monkey Face"},              // 🐵
    {"\U0001F408", "Black Cat"},                // 🐈
    {"\U0001F98C", "Deer"},                     // 🦌
    {"\U0001F344", "Mushroom"},                 // 🍄
    {"\U0001F333", "Tree"},                     // 🌳

    // Weather and Space Symbols
    {"\U0001F308", "Rainbow"},       // 🌈
    {"\U0001F320", "Shooting Star"}, // 🌠
    {"\U00002600", "Sun"},           // ☀️
    {"\U00002601", "Cloud"},         // ☁️
    {"\U000026A1", "High Voltage"},  // ⚡
    {"\U0001F525", "Fire"},          // 🔥
    {"\U000026C4", "Snowman"},       // ⛄
    {"\U0001F30A", "Water Wave"},    // 🌊

    // Transport and Map Symbols
    {"\U0001F68C", "Bus"},        // 🚌
    {"\U0001F697", "Car"},        // 🚗
    {"\U0001F6B2", "Bicycle"},    // 🚲
    {"\U0001F6A2", "Ship"},       // 🚢
    {"\U0001F681", "Helicopter"}, // 🚁
    {"\U0001F680", "Rocket"},     // 🚀
    {"\U0001F6EB", "Airplane"},   // 🛫

    // Currency Symbols
    {"\U00000024", "Dollar Sign"},     // $
    {"\U000000A3", "Pound Sign"},      // £
    {"\U000000A5", "Yen Sign"},        // ¥
    {"\U000020AC", "Euro Sign"},       // €
    {"\U0001F4B5", "Dollar Banknote"}, // 💵
    {"\U0001F4B4", "Yen Banknote"},    // 💴

    // Card Suits
    {"\U00002660", "Black Spade Suit"},   // ♠️
    {"\U00002663", "Black Club Suit"},    // ♣️
    {"\U00002665", "Black Heart Suit"},   // ♥️
    {"\U00002666", "Black Diamond Suit"}, // ♦️
    {"\U0001F0CF", "Joker Card"},         // 🃏

    // Office Supplies and Objects
    {"\U0001F4DA", "Books"},                      // 📚
    {"\U0001F4D7", "Green Book"},                 // 📗
    {"\U0001F4C8", "Chart with Upwards Trend"},   // 📈
    {"\U0001F4C9", "Chart with Downwards Trend"}, // 📉
    {"\U0001F4B0", "Money Bag"},                  // 💰
    {"\U0001F4B8", "Money with Wings"},           // 💸
    {"\U0001F4E6", "Package"},                    // 📦

    // Miscellaneous Symbols
    {"\U00002757", "Exclamation Mark"},       // ❗
    {"\U00002714", "Check Mark"},             // ✔️
    {"\U0000274C", "Cross Mark"},             // ❌
    {"\U00002705", "Check Mark Button"},      // ✅
    {"\U00002B50", "White Star"},             // ⭐
    {"\U0001F31F", "Glowing Star"},           // 🌟
    {"\U0001F4A1", "Light Bulb"},             // 💡
    {"\U0001F4A3", "Bomb"},                   // 💣
    {"\U0001F4A9", "Pile of Poo"},            // 💩
                                              // Musical Symbols
    {"\U0001F3B5", "Musical Note"},           // 🎵
    {"\U0001F3B6", "Multiple Musical Notes"}, // 🎶
    {"\U0001F3BC", "Musical Score"},          // 🎼
    {"\U0001F399", "Studio Microphone"},      // 🎙️
    {"\U0001F3A4", "Microphone"},             // 🎤

    // Food and Drink
    {"\U0001F35F", "Cheese Wedge"},   // 🧀
    {"\U0001F355", "Slice of Pizza"}, // 🍕
    {"\U0001F32D", "Taco"},           // 🌮
    {"\U0001F37D", "Beer Mug"},       // 🍻
    {"\U0001F96B", "Cup with Straw"}, // 🥤
    {"\U0001F32E", "Hot Pepper"},     // 🌶️
    {"\U0001F95A", "Potato"},         // 🥔

    // Zodiac Signs
    {"\U00002600", "Aries"},       // ♈
    {"\U00002601", "Taurus"},      // ♉
    {"\U00002602", "Gemini"},      // ♊
    {"\U00002603", "Cancer"},      // ♋
    {"\U00002604", "Leo"},         // ♌
    {"\U00002605", "Virgo"},       // ♍
    {"\U00002606", "Libra"},       // ♎
    {"\U00002607", "Scorpio"},     // ♏
    {"\U00002608", "Sagittarius"}, // ♐
    {"\U00002609", "Capricorn"},   // ♑
    {"\U0000260A", "Aquarius"},    // ♒
    {"\U0000260B", "Pisces"},      // ♓

    // Miscellaneous Shapes
    {"\U0001F4C8", "Chart Increasing"}, // 📈
    {"\U0001F4C9", "Chart Decreasing"}, // 📉
    {"\U0001F4CA", "Bar Chart"},        // 📊
    {"\U0001F7E6", "Orange Circle"},    // 🟠
    {"\U0001F7E7", "Yellow Circle"},    // 🟡
    {"\U0001F7E8", "Green Circle"},     // 🟢
    {"\U0001F7E9", "Blue Circle"},      // 🔵
    {"\U0001F7EA", "Purple Circle"},    // 🟣

    // Flags
    {"\U0001F1E6\U0001F1E9", "Flag of France"},        // 🇫🇷
    {"\U0001F1E8\U0001F1E6", "Flag of Germany"},       // 🇩🇪
    {"\U0001F1FA\U0001F1F8", "Flag of United States"}, // 🇺🇸
    {"\U0001F1E7\U0001F1F7", "Flag of Canada"},        // 🇨🇦
    {"\U0001F1EE\U0001F1F2", "Flag of Italy"},         // 🇮🇹
    {"\U0001F1F8\U0001F1EC", "Flag of Australia"},     // 🇦🇺
    {"\U0001F1F3\U0001F1F4", "Flag of Spain"},         // 🇪🇸

    // Additional Miscellaneous Symbols
    {"\U0001F4A5", "Collision"},         // 💥
    {"\U0001F4A6", "Sweat Droplets"},    // 💦
    {"\U0001F4A8", "Dashing Away"},      // 💨
    {"\U0001F50B", "Battery"},           // 🔋
    {"\U0001F4BB", "Laptop Computer"},   // 💻
    {"\U0001F4DE", "Telephone"},         // 📞
    {"\U0001F4E7", "Incoming Envelope"}, // 📧
};
size_t remo_count = sizeof(remo) / sizeof(remo[0]);

void rstrtolower(const char *input, char *output) {
    while (*input) {
        *output = tolower(*input);
        input++;
        output++;
    }
    *output = 0;
}
bool rstrinstr(const char *haystack, const char *needle) {
    char lower1[strlen(haystack) + 1];
    char lower2[strlen(needle) + 1];
    rstrtolower(haystack, lower1);
    rstrtolower(needle, lower2);
    return strstr(lower1, lower2) ? true : false;
}

void remo_print() {

    for (size_t i = 0; i < remo_count; i++) {
        printf("%s - %s\n", remo[i].str, remo[i].description);
    }
}

const char *remo_get(char *name) {
    for (size_t i = 0; i < remo_count; i++) {
        if (rstrinstr(remo[i].description, name)) {
            return remo[i].str;
        }
    }
    return NULL;
}

#endif