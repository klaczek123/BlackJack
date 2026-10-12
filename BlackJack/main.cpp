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
			if (cards[i].GetRank() == )
		}
	}


};




int main()
{
	Deck deck(1);
	cout << "Kart w talii: " << deck.CardsLeft() << endl;

	for (int i = 0; i < 5; ++i)
	{
		Card c = deck.Draw();
		cout << c.ToString() << endl;
	}

	cout << "Kart po dobraniu: " << deck.CardsLeft() << endl;

	Deck big(6);
	cout << "Kart w 6 taliach: " << big.CardsLeft() << endl;


	return 0;
}