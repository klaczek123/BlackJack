#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

enum class Suit { Hearts, Diamonds, Clubs, Spades };
enum class Rank { Two = 2, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };

class Card
{
private:
	Suit suit;
	Rank rank;


public:
	Card(Suit s, Rank r)
	{
		suit = s;
		rank = r;
	}

	Suit GetSuit() const
	{
		return suit;
	}

	Rank GetRank() const
	{
		return rank;
	}

	
	int GetValue() const
	{
		if (rank == Rank::Ace)
		{
			return 11;
		}
		if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King)
		{
			return 10;
		}
		return static_cast<int>(rank);
	}

	string ToString() const
	{
		string r;
		switch (rank)
		{
			case Rank::Jack: r = "J"; break;
				case Rank::Queen: r = "Q"; break;
					case Rank::King: r = "K"; break;
						case Rank::Ace: r = "A"; break;
						default: r = to_string(static_cast<int>(rank)); break;
		}

		string s;
		switch (suit)
		{
			case Suit::Hearts: s = "Hearts"; break;
				case Suit::Diamonds: s = "Diamonds"; break;
					case Suit::Clubs: s = "Clubs"; break;
						case Suit::Spades: s = "Spades"; break;
		}
		return r + " of " + s;
	}
};

class Deck
{
private:
	vector<Card> cards;
	int numberOfDecks;
	mt19937 rng;

public:
	Deck(int decks)
	{
		numberOfDecks = decks;
		rng.seed(random_device{}());
		Reset();
	}

	void Reset()
	{
		cards.clear();

		for (int d = 0; d < numberOfDecks; ++d)
		{
			for (int s = 0; s < 4; ++s)
			{
				for (int r = 2; r <= 14; ++r)
				{
					cards.push_back(Card(static_cast<Suit>(s), static_cast<Rank>(r)));
				}
			}
		}

		shuffle(cards.begin(), cards.end(), rng);
	}

	Card Draw()
	{
		Card c = cards.back();
		cards.pop_back();
		return c;
	}

	int CardsLeft() const
	{
		return cards.size();
	}
};

class Hand
{
private: 
	vector<Card> cards;
	int bet;

public:
	Hand(int b = 0)
	{
		bet = b;
	}

	void AddCard(const Card& card)
	{
		cards.push_back(card);
	}

	int GetScore() const
	{
		int sum = 0;
		int aces = 0;

		for (int i = 0; i < cards.size(); ++i)
		{
			sum += cards[i].GetValue();
			if (cards[i].GetRank() == Rank::Ace)
			{
				aces++;
			}
		}

		while (sum > 21 && aces > 0)
		{
			sum -= 10; // as z 11 na 1
			aces--;
		}
		return sum;
	}

	bool IsSoft() const // soft - w rece jest as liczony jako 11
	{
		int sum = 0;
		int aces = 0;

		for (int i = 0; i < cards.size(); ++i)
		{
			sum += cards[i].GetValue();
			if (cards[i].GetRank() == Rank::Ace)
			{
				aces++;
			}
		}

		while (sum > 21 && aces > 0)
		{
			sum -= 10;
			aces--;
		}
		return aces > 0; // as jako 11 w rece
	}

	bool IsBust() const
	{
		return GetScore() > 21;
	}

	bool IsBlackjack() const
	{
		return cards.size() == 2 && GetScore() == 21;
	}

	bool CanSplit() const
	{
		return cards.size() == 2 && cards[0].GetValue() == cards[1].GetValue(); // 10 i face cardy mozna splitowac
	}

	int GetBet() const
	{
		return bet;
	}

	void SetBet(int b)
	{
		bet = b;
	}

	int Size() const
	{
		return cards.size();
	}

	const Card& GetCard(int i) const
	{
		return cards[i];
	}

	string ToString() const
	{
		string result;
		for (int i = 0; i < cards.size(); ++i)
		{
			result += cards[i].ToString() + " ";
		}
		return result + "(" + to_string(GetScore()) + ")";
	}
};

class Player
{
private:
	int chips;
	vector<Hand> hands;

public:
	Player(int startingChips)
	{
		chips = startingChips;
	}

	int GetChips() const
	{
		return chips;
	}

	bool CanAfford(int amount) const
	{
		return amount <= chips;
	}

	void StartRound(int bet)
	{
		hands.clear();
		hands.push_back(Hand(bet));
		chips -= bet;
	}

	int HandCount() const
	{
		return hands.size();
	}

	Hand& GetHand(int i)
	{
		return hands[i];
	}

	const Hand& GetHand(int i) const
	{
		return hands[i];
	}

	bool DoubleDown(int handIndex)
	{
		int extra = hands[handIndex].GetBet();
		if (!CanAfford(extra))
		{
			return false;
		}

		chips -= extra;
		hands[handIndex].SetBet(extra * 2);
		return true;
	}

	bool Split(int handIndex)
	{
		Hand& h = hands[handIndex];
		int bet = h.GetBet();

		if (!h.CanSplit() || !CanAfford(bet))
		{
			return false;
		}

		Card second = h.GetCard(1);
		Hand first(bet);
		first.AddCard(h.GetCard(0));
		Hand other(bet);
		other.AddCard(second);

		hands[handIndex] = first;
		hands.push_back(other);
		chips -= bet;
		return true;
	}

	void Pay(int amount)
	{
		chips += amount;
	}
};


int main()
{
	
		Player p(1000);
		p.StartRound(100);
		cout << "Zetony: " << p.GetChips() << endl;   // 900

		p.GetHand(0).AddCard(Card(Suit::Hearts, Rank::Eight));
		p.GetHand(0).AddCard(Card(Suit::Spades, Rank::Eight));
		cout << p.GetHand(0).ToString() << endl;       // 8H 8S (16)

		if (p.Split(0))
		{
			cout << "Rece: " << p.HandCount() << endl;       // 2
			cout << "Zetony: " << p.GetChips() << endl;      // 800
			cout << p.GetHand(0).ToString() << endl;         // 8H (8)
			cout << p.GetHand(1).ToString() << endl;         // 8S (8)
		}


	return 0;
}