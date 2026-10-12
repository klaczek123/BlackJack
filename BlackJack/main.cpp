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
			case Suit::Hearts: s = "Kier"; break;
				case Suit::Diamonds: s = "Karo"; break;
					case Suit::Clubs: s = "Trefl"; break;
						case Suit::Spades: s = "Pik"; break;
		}
		return r + " " + s;
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

	int TotalCards() const
	{
		return numberOfDecks * 52;
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
class Participant
{
protected:
	vector<Hand> hands;

public:
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

	void ClearHands()
	{
		hands.clear();
	}
};

class Gambler : public Participant
{
private:
	int chips;

public:
	Gambler(int startingChips)
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

class Dealer : public Participant
{
public:
	void StartRound()
	{
		hands.clear();
		hands.push_back(Hand(0));
	}
	bool ShouldHit() const
	{
		return hands[0].GetScore() < 17;
	}
};

class Game
{
private:
	Deck deck;
	Gambler player;
	Dealer dealer;

	void CheckDeck()
	{
		if (deck.CardsLeft() < deck.TotalCards() / 4)   // gdy zostanie mniej niz 25% kart - tasowanie
		{
			deck.Reset();
			cout << "[Talia przetasowana]" << endl;
		}
	}

	void ShowTable() const
	{
		cout << "\nDealer: " << dealer.GetHand(0).ToString() << endl;

		for (int i = 0; i < player.HandCount(); ++i)
		{
			cout << "Twoja reka " << i + 1 << ": " << player.GetHand(i).ToString()
				<< " zaklad: " << player.GetHand(i).GetBet() << endl;
		}
		cout << "Zetony: " << player.GetChips() << endl;
	}

	void PlayHand(int i)
	{
		while (true)
		{
			Hand& h = player.GetHand(i);

			if (h.IsBust() || h.GetScore() == 21)
			{
				return;
			}

			ShowTable();
			cout << "Twoja reka " << i + 1 << ": [h]it, [s]tand";
			bool canDouble = h.Size() == 2 && player.CanAfford(h.GetBet());
			bool canSplit = h.CanSplit() && player.CanAfford(h.GetBet());
			if (canDouble) cout << ", [d]ouble down";
			if (canSplit) cout << ", s[p]lit";
			cout << ": ";

			char choice;
			cin >> choice;

			if (choice == 'h')
			{
				h.AddCard(deck.Draw());
			}
			else if (choice == 's')
			{
				return;
			}
			else if (choice == 'd' && canDouble)
			{
				player.DoubleDown(i);
				player.GetHand(i).AddCard(deck.Draw());
				return;
			}
			else if (choice == 'p' && canSplit)
			{
				player.Split(i);
				player.GetHand(i).AddCard(deck.Draw());
				player.GetHand(player.HandCount() - 1).AddCard(deck.Draw());
			}
			else
			{
				cout << "Niepoprawny wybor" << endl;
			}
		}
	}

public:
	Game(int decks, int startingChips) : deck(decks), player(startingChips) {}

	bool PlayRound()
	{
		CheckDeck();

		int bet;
		do
		{
			cout << "\nZetony: " << player.GetChips() << ". Zaklad (0 = koniec): ";
			cin >> bet;
		} while (bet < 0 || !player.CanAfford(bet));

		if (bet == 0)
		{
			return false;
		}

		player.StartRound(bet);
		dealer.StartRound();

		player.GetHand(0).AddCard(deck.Draw());
		dealer.GetHand(0).AddCard(deck.Draw());
		player.GetHand(0).AddCard(deck.Draw());

		if (player.GetHand(0).IsBlackjack())
		{
			int bjValue = dealer.GetHand(0).GetCard(0).GetValue();

			if (bjValue == 10 || bjValue == 11)
			{
				cout << "Dealer dobiera druga karte..." << endl;
				dealer.GetHand(0).AddCard(deck.Draw());
				ShowTable();

				if (dealer.GetHand(0).IsBlackjack())
				{
					cout << "Dealer BlackJack: Push" << endl;
					player.Pay(bet);
				}
				else
				{
					cout << "Blackjack! Wyplata 3:2" << endl;
					player.Pay(bet * 5 / 2);
				}
			}
			else
			{
				ShowTable();
				cout << "Blackjack! Wyplata 3:2" << endl;
				player.Pay(bet * 5 / 2);
			}
			return true;
		}

		for (int i = 0; i < player.HandCount(); ++i)
		{
			PlayHand(i);
		}

		bool anyAlive = false;
		for (int i = 0; i < player.HandCount(); ++i)
		{
			if (!player.GetHand(i).IsBust())
			{
				anyAlive = true;
			}
		}

		if (anyAlive)
		{
			dealer.GetHand(0).AddCard(deck.Draw());

			while (dealer.ShouldHit())
			{
				dealer.GetHand(0).AddCard(deck.Draw());
			}
		}

		ShowTable();

		int dealerScore = dealer.GetHand(0).GetScore();
		bool dealerBust = dealer.GetHand(0).IsBust();
		bool dealerBJ = dealer.GetHand(0).IsBlackjack();

		if (dealerBJ)
		{
			cout << "Dealer ma Blackjacka" << endl;
		}

		for (int i = 0; i < player.HandCount(); ++i)
		{
			const Hand& h = player.GetHand(i);
			int bt = h.GetBet();
			cout << "Reka " << i + 1 << ": ";

			if (h.IsBust())
			{
				cout << "Bust, za duzo" << endl;
			}
			else if (dealerBJ)
			{
				cout << "przegrana, Blackjack dealera" << endl;
			}
			else if (dealerBust || h.GetScore() > dealerScore)
			{
				cout << "Wygrana!" << endl;
				player.Pay(bt * 2);
			}
			else if (h.GetScore() == dealerScore)
			{
				cout << "Push" << endl;
				player.Pay(bt);
			}
			else
			{
				cout << "przegrana" << endl;
			}
		}
		return true;
	}
};

int main()
{
	Game game(6, 1000);

	while (game.PlayRound()){}
	
	cout << "Koniec gry." << endl;


	return 0;
}