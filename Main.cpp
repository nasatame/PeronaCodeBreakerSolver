
# include <Siv3D.hpp> // OpenSiv3D v0.4.3

struct MetalPoint {
	int number;
	int state;
	Rect r;

	MetalPoint() : MetalPoint(0)
	{}

	MetalPoint(int num) {
		number = num;
		r = Rect((number % 5) * 110, (number / 5) * 110, 100, 100);
		state = 1;
	}
	void draw(Font f) const {
		if (number == 24 || number == 23 || number == 19 || number == 18)
			return;
		r.draw(Palette::Gray);
		f(state).drawAt(r.center(), Palette::Red);
	}
	void update() {
		if (r.leftClicked()) {
			state++;
		}
		if (r.rightClicked()) {
			state--;
		}
		if (state < 0) {
			state = 9;
		}
		if (state > 9) {
			state = 0;
		}
	}
};

using answer_sheet = std::array<std::array<short, 3>, 3>;

std::vector<answer_sheet> sheets;

//9! = 360,000
void MakeAnswer() {

	std::vector<int> per;

	for (int i = 0; i < 9; i++)
		per.push_back(i + 1);

	answer_sheet s;

	do {

		for (int i = 0; i < 9; i++) {
			s[i % 3][i / 3] = per[i];
		}

		sheets.push_back(s);

	} while (std::next_permutation(per.begin(), per.end()));

}

bool Checker(const Grid<MetalPoint>& card, answer_sheet ans) {

	//横パターン
	for (int i = 0; i < 3; i++) {
		int a[3] = { card[i][0].state,card[i][1].state,card[i][2].state },
			b[3] = { ans[i][0],ans[i][1] ,ans[i][2] };

		int hit = 0;
		int ball = 0;

		for (int k = 0; k < 3; k++) {
			if (a[k] == b[k]) {
				hit++;
			}
			else if (a[k] == b[0] || a[k] == b[1] || a[k] == b[2]) {
				ball++;
			}
		}

		if (hit != card[i][3].state || ball != card[i][4].state) {
			return false;
		}

	}
	//縦パターン
	for (int i = 0; i < 3; i++) {
		int a[3] = { card[0][i].state,card[1][i].state,card[2][i].state },
			b[3] = { ans[0][i],ans[1][i] ,ans[2][i] };

		int hit = 0;
		int ball = 0;

		for (int k = 0; k < 3; k++) {
			if (a[k] == b[k]) {
				hit++;
			}
			else if (a[k] == b[0] || a[k] == b[1] || a[k] == b[2]) {
				ball++;
			}
		}

		if (hit != card[3][i].state || ball != card[4][i].state) {
			return false;
		}
	}

	return true;
}

//5x5 and 3x3
//return answer num
std::vector<int> MetalCardChecker(const Grid<MetalPoint>& card) {
	std::vector<int> result;

	for (int i = 0; i < sheets.size(); i++) {
		if (Checker(card, sheets[i])) {
			result.push_back(i);
		}
	}

	return result;
}

std::vector<int> MetalCardChecker(const std::vector<Grid<MetalPoint>>& cards) {
	std::vector<int> result;
	std::vector<int> next;

	for (int i = 0; i < sheets.size(); i++) {
		result.push_back(i);
	}

	for (int k = 0; k < cards.size(); k++) {
		for (int i = 0; i < result.size(); i++) {
			if (Checker(cards[k], sheets[result[i]])) {
				next.push_back(result[i]);
			}
		}
		result = std::move(next);
		next = std::vector<int>();
	}

	return result;
}

int AnswerRecommend(Grid<MetalPoint> grid, std::vector<int> answers) {


	//スコア評価

	std::vector<std::pair<int, int>> list;
	for (int k = 0; k < answers.size(); k++) {
		int score = 0;
		for (int y = 0; y < 3; y++) {
			for (int x = 0; x < 3; x++) {
				score += std::min(abs(sheets[answers[k]][y][x] - grid[y][x].state),9 - abs(sheets[answers[k]][y][x] - grid[y][x].state));
			}
		}
		list.push_back(std::pair<int, int>(score, answers[k]));
	}



	sort(list.begin(), list.end());

	return list[0].second;
}

void Main()
{

	Grid<MetalPoint> grid(5, 5);
	std::vector<Grid<MetalPoint>> stock;

	Font font(30);

	int i = 0;
	for (auto& t : grid) {
		t = MetalPoint(i);
		i++;
	}

	for (int i = 0; i < 9; i++) {
		grid[i / 3][i % 3].state = i + 1;
	}

	String solve_label = U"solve";
	String reset_label = U"reset";

	String answer = U"";

	std::vector<int> answer_list;
	MakeAnswer();

	while (System::Update())
	{
		for (auto& t : grid) {
			t.update();
		}
		for (const auto& t : grid) {
			t.draw(font);
		}

		if (SimpleGUI::Button(reset_label, Point(600, 50))) {
			stock.clear();
			answer = U"";

			for (int i = 0; i < 9; i++) {
				grid[i / 3][i % 3].state = i + 1;
			}
		}

		if (SimpleGUI::Button(solve_label, Point(600, 100))) {
			stock.push_back(grid);
			answer_list = MetalCardChecker(stock);

			if (answer_list.size() == 0) {
				answer = U"";
			}
			else {
				answer = U"";
				int recommended = AnswerRecommend(grid, answer_list);
				answer += U"NUM:" + ToString(answer_list.size());
				for (int i = 0; i < 9; i++) {
					if (i % 3 == 0) {
						answer += U"\n";
					}
					answer += ToString(sheets[recommended][i / 3][i % 3]);
				}
			}

		}

		font(answer).draw(600, 200, Palette::White);

	}
}

