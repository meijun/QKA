#include <iostream>
#include <array>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>

using namespace std;

template<class T>
class Comparable {
public:
    friend bool operator<=(const T &left, const T &right) {
        return !(right < left);
    }

    friend bool operator>(const T &left, const T &right) {
        return right < left;
    }

    friend bool operator>=(const T &left, const T &right) {
        return !(left < right);
    }

    friend bool operator==(const T &left, const T &right) {
        return !(left < right) && !(right < left);
    }

    friend bool operator!=(const T &left, const T &right) {
        return left < right || right < left;
    }
};

enum class Color {
    kPike,
    kHeart,
    kClover,
    kTile,
};

class Card : public Comparable<Card> {
public:
    Card(int number, Color color) : number_(number), color_(color) {}

    bool operator<(const Card &other) const {
        if (number_ != other.number_) {
            return number_ < other.number_;
        }
        return color_ < other.color_;
    }

    string ToString() const {
        string number_str;
        switch (number_) {
            case 1:
                number_str = "A";
                break;
            case 11:
                number_str = "J";
                break;
            case 12:
                number_str = "Q";
                break;
            case 13:
                number_str = "K";
                break;
            default:
                number_str = to_string(number_);
        }
        string color_str;
        switch (color_) {
            case Color::kPike:
                color_str = "P";
                break;
            case Color::kHeart:
                color_str = "H";
                break;
            case Color::kClover:
                color_str = "C";
                break;
            case Color::kTile:
                color_str = "T";
                break;
        }
        return number_str + "_" + color_str;
    }

    int number_;
    Color color_;
};

enum class Type {
    kHighCard,
    kPair,
    kFlush,
    kStraight,
    kStraightFlush,
    kBomb,
};

class Hand : public Comparable<Hand> {
public:
    explicit Hand(array<Card, 3> cards) : cards_(cards) {
        sort(cards_.begin(), cards_.end());
    }

    bool operator<(const Hand &other) const {
        return ToScore() < other.ToScore();
    }

    vector<int> ToScore() const {
        array<int, 3> a1 = {cards_[0].number_, cards_[1].number_, cards_[2].number_};
        array<int, 3> a14 = a1;
        for (int i = 0; i < 3; i++) {
            if (a14[i] == 1) {
                a14[i] = 14;
            }
        }
        sort(a14.begin(), a14.end());
        if (a1[0] == a1[1] && a1[0] == a1[2]) {
            return {static_cast<int>(Type::kBomb), a14[0]};
        }
        bool flush = cards_[0].color_ == cards_[1].color_ && cards_[0].color_ == cards_[2].color_;
        bool straight = (a1[0] + 1 == a1[1] && a1[0] + 2 == a1[2]) || (a14[0] + 1 == a14[1] && a14[0] + 2 == a14[2]);
        if (flush && straight) {
            return {static_cast<int>(Type::kStraightFlush), a14[2]};
        }
        if (straight) {
            return {static_cast<int>(Type::kStraight), a14[2]};
        }
        if (flush) {
            return {static_cast<int>(Type::kFlush), a14[2], a14[1], a14[0]};
        }
        if (a14[0] == a14[1]) {
            return {static_cast<int>(Type::kPair), a14[0], a14[2]};
        }
        if (a14[1] == a14[2]) {
            return {static_cast<int>(Type::kPair), a14[1], a14[0]};
        }
        return {static_cast<int>(Type::kHighCard), a14[2], a14[1], a14[0]};
    }

    string ToString() const {
        stringstream result;
        bool first = true;
        for (const auto &card : cards_) {
            if (first) {
                first = false;
            } else {
                result << ",";
            }
            result << setw(4) << card.ToString();
        }
        return result.str();
    }

    array<Card, 3> cards_;
};

int main() {
    vector<Card> cards;
    for (int number = 1; number <= 13; number++) {
        cards.emplace_back(number, Color::kPike);
        cards.emplace_back(number, Color::kHeart);
        cards.emplace_back(number, Color::kClover);
        cards.emplace_back(number, Color::kTile);
    }
    vector<Hand> hands;
    for (const auto &card0 : cards) {
        for (const auto &card1 : cards) {
            for (const auto &card2 : cards) {
                if (card0 != card1 && card0 != card2 && card1 != card2) {
                    hands.push_back(Hand({card0, card1, card2}));
                }
            }
        }
    }
    cout << hands.size() << endl;
    for (int i = 0; i < 10; i++) {
        int k = i * 1000000007 % hands.size();
        cout << hands[k].ToString() << ": ";
        for (const auto &score : hands[k].ToScore()) {
            cout << score << " ";
        }
        cout << endl;
    }
    sort(hands.begin(), hands.end());
    for (double p : {0.999, 0.99, 0.95, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.05, 0.01, 0.001}) {
        int i = lround(p * (hands.size() - 1.0));
        cout << setw(5) << p << ": " << setw(6) << i << " " << hands[i].ToString() << endl;
    }
    return 0;
}
