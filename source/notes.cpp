#include "header.h"
#include "main.h"
#include "time.h"
#include "audio.h"
#include "tja.h"
#include "notes.h"
#include "score.h"
#include "vorbis.h"
#include "option.h"
#define AUTO_ROLL_FRAME comboVoice //オート時の連打の間隔

int balloon[4][256],BalloonCount[4],TotalFailedCount,
NowMeCount,dcd,JBS = -1,bid = 0,id = -1,TJAVER = 1,TextCnt = 0;
double bnc = 512,nc = 0,bpm,offset;
float NowBPM = 120.0f;
extern int isBranch, comboVoice, course, stme;
extern double black;
C2D_Font font;

int find_notes_id(), find_line_id(), make_roll_start(int NotesId), make_roll_end(int NotesId),
make_balloon_start(int NotesId), sign(double A), make_balloon_end(int NotesId);
void init_notes(TJA_HEADER_T TJA_Header), draw_judge(double CurrentTimeNotes, C2D_Sprite sprites[SPRITES_NUMER]), notes_sort(), delete_roll(int i),
notes_draw(C2D_Sprite sprites[SPRITES_NUMER]), make_balloon_break(), delete_notes(int i), draw_lyric_text(const char *text, float x, float y, float size),
notes_calc(int isDon, int isKatsu, double bpm, double CurrentTimeNotes, int cnt, C2D_Sprite sprites[SPRITES_NUMER], MEASURE_T Measure[MEASURE_MAX]);

std::vector<NOTES_T> Notes;
COMMAND_T Command;
BARLINE_T BarLine[BARLINE_MAX];
ROLL_T RollNotes[ROLL_MAX];
BALLOON_T BalloonNotes[BALLOON_MAX];
BRANCH_T Branch;

int MeasureCount, MinMeasureCount, MaxMeasureCount, RollState, NotesCount, JudgeDispknd, JudgeRollState, BalloonBreakCount, PreNotesKnd,
NotesNumber;	//何番目のノーツか
bool  isNotesLoad = true,isJudgeDisp = false,isBalloonBreakDisp = false,isGOGOTime = false,isLevelHold = false;	//要初期化
double JudgeMakeTime,JudgeY,JudgeEffectCnt;


void notes_main(int isDon,int isKatsu,char tja_notes[MEASURE_MAX][NOTES_MEASURE_MAX],MEASURE_T Measure[MEASURE_MAX],int cnt,C2D_Sprite sprites[SPRITES_NUMER]) {

	OPTION_T Option;
	get_option(&Option);
	TextCnt = 0;

	//最初の小節のcreate_timeがマイナスだった時用に調整
	double CurrentTimeNotes = Measure[stme].create_time;
	if (cnt >= 0) CurrentTimeNotes = get_current_time(TIME_NOTES) + Measure[stme].create_time;
	if (cnt == 0) Branch.course = Measure[stme].branch;
	//snprintf(get_buffer(), BUFFER_SIZE, "fmt:%.4f ctm:%.2f ct:%.2f 0ct:%.4f", get_FirstMeasureTime(), CurrentTimeNotes, CurrentTimeNotes - Measure[0].create_time, Measure[stme].create_time);
	//draw_debug(0, 185, get_buffer());

	if (isNotesLoad) {

		//分岐
		if (Branch.next) {

			while (MeasureCount < MEASURE_MAX) {

				++MeasureCount;
				break;
			}
			Branch.next = false;
		}

		while (Measure[MeasureCount].create_time <= CurrentTimeNotes && !Branch.wait) {

			NotesCount = 0;

			if (Measure[MeasureCount].branch != Branch.course && Measure[MeasureCount].branch != -1) {

				++MeasureCount;
				continue;
			}

			//ノーツ数の計測
			while (isNotesLoad && tja_notes[Measure[MeasureCount].notes][NotesCount] != ',' && tja_notes[Measure[MeasureCount].notes][NotesCount] != '\n' && tja_notes[Measure[MeasureCount].notes][NotesCount] != '/') {

				//生成時に発動する命令
				if (NotesCount == 0 && tja_notes[Measure[MeasureCount].notes][0] == '#' && (Measure[MeasureCount].branch == Branch.course || Measure[MeasureCount].branch == -1)) {

					get_command_value(tja_notes[Measure[MeasureCount].notes], &Command);
					Command.notes = tja_notes[Measure[MeasureCount].notes];

					switch (Command.knd) {
					case COMMAND_END:
						isNotesLoad = false;
						break;
					case COMMAND_BRANCHSTART:

						if (!isLevelHold) {

							Branch.knd = Command.val[0];
							Branch.x = Command.val[1];
							Branch.y = Command.val[2];
							Branch.wait = true;
						}
						break;
					case COMMAND_BRANCHEND:
						Branch.course = -1;
						break;
					}
					NotesCount = 0;
					++MeasureCount;
					if (Branch.wait) break;
					else continue;
				}
				++NotesCount;
			}

			if (isNotesLoad == false || Branch.wait) break;

			//小節線
			int BarLineId = find_line_id();
			if (BarLineId != -1 && Measure[MeasureCount].branch == Branch.course) {
				BarLine[BarLineId].flag = true;
				BarLine[BarLineId].scroll = Measure[MeasureCount].scroll * Option.speed;
				BarLine[BarLineId].measure = MeasureCount;
				BarLine[BarLineId].x_ini = NOTES_JUDGE_RANGE * BarLine[BarLineId].scroll + NOTES_JUDGE_X;
				BarLine[BarLineId].create_time = CurrentTimeNotes;
				BarLine[BarLineId].isDisp = Measure[MeasureCount].isDispBarLine;
				if (Measure[MeasureCount].judge_time < Measure[MinMeasureCount].judge_time) BarLine[BarLineId].flag = false;
			}

			int NotesCountMax;

			if (Measure[MeasureCount].firstmeasure != -1 && get_MeasureId_From_OriginalId(Measure[MeasureCount].firstmeasure) != -1) {

				NotesCountMax = Measure[get_MeasureId_From_OriginalId(Measure[MeasureCount].firstmeasure)].max_notes;
			}
			else {
				NotesCountMax = NotesCount;
			}

			notes_sort();	//ソート
			bid = find_notes_id();
			for (int i = 0; i < NotesCount; ++i) {

				if (ctoi(tja_notes[Measure[MeasureCount].notes][i]) != 0 && Measure[MeasureCount].branch == Branch.course) {

					id = find_notes_id();
					int knd = ctoi(tja_notes[Measure[MeasureCount].notes][i]);
					if ((knd == NOTES_ROLL || knd == NOTES_BIGROLL || knd == NOTES_BALLOON) && (PreNotesKnd == knd)) {	//55558のような表記に対応
						continue;
					}
					if (Measure[MeasureCount].judge_time < Measure[MinMeasureCount].judge_time && knd == NOTES_BALLOON && Measure[MeasureCount].branch == -1) ++BalloonCount[0];
					if (Measure[MeasureCount].judge_time < Measure[MinMeasureCount].judge_time && knd == NOTES_BALLOON && Measure[MeasureCount].branch != -1) {
						++BalloonCount[1];
						++BalloonCount[2];
						++BalloonCount[3];
					}

					if (Measure[MeasureCount].judge_time < Measure[MinMeasureCount].judge_time) continue;

					if (Option.random > 0) {		//ランダム(きまぐれ,でたらめ)
						if (rand() % 100 < Option.random * 100) {
							switch (knd) {
							case NOTES_DON: knd = NOTES_KATSU; break;
							case NOTES_KATSU: knd = NOTES_DON; break;
							case NOTES_BIGDON: knd = NOTES_BIGKATSU; break;
							case NOTES_BIGKATSU: knd = NOTES_BIGDON; break;
							}
						}
					}
					if (Option.isSwap) {	//あべこべ
						switch (knd) {
						case NOTES_DON: knd = NOTES_KATSU; break;
						case NOTES_KATSU: knd = NOTES_DON; break;
						case NOTES_BIGDON: knd = NOTES_BIGKATSU; break;
						case NOTES_BIGKATSU: knd = NOTES_BIGDON; break;
						}
					}

					Notes[id].flag = true;
					Notes[id].notes_max = NotesCount;
					Notes[id].num = NotesNumber;
					Notes[id].scroll = Measure[MeasureCount].scroll*Option.speed;
					Notes[id].x_ini = NOTES_JUDGE_RANGE*Notes[id].scroll+NOTES_JUDGE_X;
					Notes[id].bpm = Measure[MeasureCount].bpm;
					Notes[id].knd = knd;
					Notes[id].x = Notes[id].x_ini;
					double NoteTime = 240.0/Measure[MeasureCount].bpm*Measure[MeasureCount].measure*i/NotesCountMax;
					//Notes[id].create_time = CurrentTimeNotes;
					Notes[id].pop_time = Measure[MeasureCount].pop_time+NoteTime;
					Notes[id].judge_time = Measure[MeasureCount].judge_time+NoteTime;
					Notes[id].roll_id = -1;
					Notes[id].isThrough = false;

					PreNotesKnd = knd;

					int roll_id = -1;

					switch (Notes[id].knd) {

					case NOTES_ROLL:
						RollState = NOTES_ROLL;
						roll_id = make_roll_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case NOTES_BIGROLL:
						RollState = NOTES_BIGROLL;
						roll_id = make_roll_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case NOTES_BALLOON:
						RollState = NOTES_BALLOON;
						roll_id = make_balloon_start(id);
						if (roll_id != -1) {
							Notes[id].roll_id = roll_id;
						}
						else {
							delete_notes(id);
						}
						break;

					case NOTES_ROLLEND:

						switch (RollState) {
						case NOTES_ROLL:
							roll_id = make_roll_end(id);
							if (roll_id != -1) {
								Notes[id].roll_id = roll_id;
								Notes[id].knd = NOTES_ROLLEND;
								RollState = 0;
							}
							else {
								delete_notes(id);
							}
							break;

						case NOTES_BIGROLL:
							roll_id = make_roll_end(id);
							if (roll_id != -1) {
								Notes[id].roll_id = roll_id;
								Notes[id].knd = NOTES_BIGROLLEND;
								RollState = 0;
							}
							else {
								delete_notes(id);
							}
							break;

						case NOTES_BALLOON:
							roll_id = make_balloon_end(id);
							if (roll_id != -1) {
								BalloonNotes[roll_id].end_id = id;
								Notes[id].roll_id = roll_id;
								Notes[id].knd = NOTES_BALLOONEND;
								RollState = 0;
							}
							else {
								delete_notes(id);
							}
							break;

						default:
							Notes[id].flag = false;
							break;
						}
						RollState = 0;
						break;
					}
					if (TJAVER < 2 && bid != id) {
						switch (Notes[bid].knd) {
						case NOTES_DON:
						case NOTES_BOMB:
							Notes[bid].text_id = 3;
							if ((nc <= 0.125 && nc <= bnc) || nc <= (1.0/12.0)) Notes[bid].text_id = 1;
							break;
						case NOTES_KATSU:
							Notes[bid].text_id = 5;
							if ((nc <= 0.125 && nc <= bnc) || nc <= (1.0/12.0)) Notes[bid].text_id = 4;
							break;
						case NOTES_BIGDON:
							Notes[bid].text_id = 6;
							break;
						case NOTES_BIGKATSU:
							Notes[bid].text_id = 7;
							break;
						case NOTES_ROLL:
							Notes[bid].text_id = 8;
							break;
						case NOTES_BIGROLL:
							Notes[bid].text_id = 9;
							break;
						case NOTES_BALLOON:
							Notes[bid].text_id = 12;
							break;
						case NOTES_ROLLEND:
						case NOTES_BIGROLLEND:
							Notes[bid].text_id = 11;
							break;
						case NOTES_BALLOONEND:
							Notes[bid].text_id = 0;
							break;
						}
						bnc = nc;
					}
					else if (TJAVER >= 2) {
						switch (tja_notes[Measure[MeasureCount].notes][i]) {
						case 'C':
						case 'D':
							if (Notes[id].knd == NOTES_DON || Notes[id].knd == NOTES_BOMB) Notes[id].text_id = 1;
							else Notes[id].text_id = 4;
							break;
						case 'E':
							if (Notes[id].knd == NOTES_DON) Notes[id].text_id = 2;
							else Notes[id].text_id = 4;
							break;
						case 'F':
							if (Notes[id].knd == NOTES_DON) Notes[id].text_id = 3;
							else Notes[id].text_id = 5;
							break;
						case 'G':
							if (Notes[id].knd == NOTES_KATSU) Notes[id].text_id = 4;
							else Notes[id].text_id = 1;
							break;
						case 'H':
							if (Notes[id].knd == NOTES_KATSU) Notes[id].text_id = 5;
							else Notes[id].text_id = 3;
							break;
						case 'A':
						case 'I':
							if (Notes[id].knd == NOTES_BIGDON) Notes[id].text_id = 6;
							else Notes[id].text_id = 7;
							break;
						case 'B':
						case 'J':
							if (Notes[id].knd == NOTES_BIGKATSU) Notes[id].text_id = 7;
							else Notes[id].text_id = 6;
							break;
						case 'K':
							Notes[id].text_id = 8;
							break;
						case 'L':
							Notes[id].text_id = 9;
							break;
						case 'M':
							Notes[id].text_id = 12;
							break;
						case 'N':
							if (Notes[id].knd == NOTES_BALLOONEND) Notes[id].text_id = 0;
							else Notes[id].text_id = 11;
							break;
						}
					}
					++NotesNumber;
					bid = id;
					nc = Measure[MeasureCount].measure / NotesCountMax;
				}
				else nc += Measure[MeasureCount].measure / NotesCountMax;
			}
			if (NotesCount != NotesCountMax) nc -= Measure[MeasureCount].measure / NotesCountMax;
			if (TJAVER < 2) {
				switch (Notes[bid].knd) {
				case NOTES_DON:
				case NOTES_BOMB:
					Notes[bid].text_id = 3;
					if ((nc <= 0.125 && nc <= bnc) || nc <= (1.0/12.0)) Notes[bid].text_id = 1;
					bnc = nc;
					break;
				case NOTES_KATSU:
					Notes[bid].text_id = 5;
					if ((nc <= 0.125 && nc <= bnc) || nc <= (1.0/12.0)) Notes[bid].text_id = 4;
					bnc = nc;
					break;
				case NOTES_BIGDON:
					Notes[bid].text_id = 6;
					bnc = nc;
					break;
				case NOTES_BIGKATSU:
					Notes[bid].text_id = 7;
					bnc = nc;
					break;
				case NOTES_ROLL:
					Notes[bid].text_id = 8;
					bnc = nc;
					break;
				case NOTES_BIGROLL:
					Notes[bid].text_id = 9;
					bnc = nc;
					break;
				case NOTES_BALLOON:
					Notes[bid].text_id = 12;
					bnc = nc;
					break;
				case NOTES_ROLLEND:
				case NOTES_BIGROLLEND:
					Notes[bid].text_id = 11;
					bnc = nc;
					break;
				case NOTES_BALLOONEND:
					Notes[bid].text_id = 0;
					bnc = nc;
					break;
				}
			}
			if (NotesCount != NotesCountMax) nc += Measure[MeasureCount].measure / NotesCountMax;
			if (NotesCount == 0) nc += Measure[MeasureCount].measure;
			++MeasureCount;
			notes_sort();	//ソート
		}
	}

	if (cnt < 0) return;
	for (int i = 0, j = BARLINE_MAX - 1; i < j; ++i) {

		if (BarLine[i].flag) {

			BarLine[i].x = BarLine[i].x_ini -
				NOTES_AREA * BarLine[i].scroll * (CurrentTimeNotes - Measure[BarLine[i].measure].pop_time) * (Measure[BarLine[i].measure].bpm / 240.0);

			if (BarLine[i].isDisp) {
				C2D_DrawRectSolid(BarLine[i].x, 86, 0, 1, 46, C2D_Color32f(1, 1, 1, 1));

				//snprintf(buf_notes, sizeof(buf_notes), "%d", Measure[BarLine[i].measure].branch);
				//draw_debug(BarLine[i].x - 10, 133, buf_notes);
			}
			if ((BarLine[i].x < 62 && BarLine[i].scroll > 0) || (BarLine[i].x > 400 && BarLine[i].scroll < 0)) BarLine[i].flag = false;
		}
	}

	if (!get_isPause()) notes_calc(isDon, isKatsu, bpm, CurrentTimeNotes, cnt, sprites, Measure);
	if (!Option.isStelth) notes_draw(sprites);
	draw_emblem(sprites);
	draw_judge(CurrentTimeNotes, sprites);
	
	if (MaxMeasureCount < MeasureCount) MaxMeasureCount = MeasureCount;

	for (int i = 0, j = MaxMeasureCount; i < j; ++i) {	//判定時に発動する命令

		if ((Measure[i].branch == Branch.course || Measure[i].branch == -1) && Measure[i].flag) {

			bool NotFalse = false;

			if (Measure[i].command != -1 && Measure[i].judge_time <= CurrentTimeNotes) {

				switch (Measure[i].command) {
				case COMMAND_GOGOSTART:
					isGOGOTime = true;
					break;
				case COMMAND_GOGOEND:
					isGOGOTime = false;
					break;
				case COMMAND_SECTION:
					init_branch_section();
					break;
				case COMMAND_BRANCHSTART:

					if (isLevelHold == false && (Branch.knd != 0 || JudgeRollState == -1)) {	//連打分岐の時は連打が無くなってから分岐

						Branch.course = start_branch(Branch.knd, Branch.x, Branch.y);
						Branch.next = true;
						Branch.wait = false;
					}
					else NotFalse = true;
					break;
				case COMMAND_LEVELHOLD:
					isLevelHold = true;
					break;
				default:
					break;
				}
			}
			if (NotFalse == false && Measure[i].judge_time <= CurrentTimeNotes) Measure[i].flag = false;
		}
	}
	send_gogotime(isGOGOTime);
	double MaxJudgeTime = 0.0;
	int NowMeasure = 0;

	for (int i = 0, j = MaxMeasureCount; i < j; ++i) {
		if (Measure[i].command == -1 &&
			MaxJudgeTime < Measure[i].judge_time && Measure[i].judge_time <= CurrentTimeNotes) {
			NowBPM = Measure[i].bpm;
			MaxJudgeTime = Measure[i].judge_time;
			NowMeasure = i;
		}
	}

	draw_lyric_text(Measure[NowMeasure].lyric.data(), 200, 222, 0.6);
	if (course == COURSE_DAN) dcd = dan_condition();
	if (TotalFailedCount != dcd) {
		play_sound(SOUND_FAILED);
		TotalFailedCount = dcd;
	}
	
	/*snprintf(get_buffer(), BUFFER_SIZE, "cnt :%d", cnt);
	draw_debug(100, 0, get_buffer());
	snprintf(get_buffer(), BUFFER_SIZE, "Bpm:%.1f     Measure:%.1f     Scroll:%.1f", Measure[MeasureCount].bpm, Measure[MeasureCount].measure, Measure[MeasureCount].scroll);
	draw_debug(0, 20, get_buffer());
	snprintf(get_buffer(), BUFFER_SIZE, "Judge:%.3f Create:%.3f Pop:%.3f", Measure[MeasureCount].judge_time, Measure[MeasureCount].create_time, Measure[MeasureCount].pop_time);
	draw_debug(0, 40, get_buffer());
	snprintf(get_buffer(), BUFFER_SIZE, "%d: %s", MeasureCount, tja_notes[MeasureCount]);
	draw_debug(0, 50, get_buffer());
	snprintf(get_buffer(), BUFFER_SIZE, "course:%d", Branch.course);
	draw_debug(250, 40, get_buffer());*/
}

int find_notes_id() {

	for (int i = 0, j = Notes.size() - 1; i < j; ++i) {
		if (!Notes[i].flag) return i;
	}
	Notes.resize(Notes.size() * 2);
	for (int i = 0, j = Notes.size() - 1; i < j; ++i) {
		if (!Notes[i].flag) return i;
	}
	return -1;
}

int find_line_id() {

	for (int i = 0, j = BARLINE_MAX - 1; i < j; i += 4) {
		if (!BarLine[i].flag) return i;
		if (!BarLine[i+1].flag) return i+1;
		if (!BarLine[i+2].flag) return i+2;
		if (!BarLine[i+3].flag) return i+3;
	}
	return -1;
}

void make_judge(int knd, double CurrentTimeNotes) {
	isJudgeDisp = true;
	JudgeMakeTime = CurrentTimeNotes;
	JudgeDispknd = knd;
	JudgeY = 73;
	JudgeEffectCnt = 0;
}

void draw_judge(double CurrentTimeNotes, C2D_Sprite sprites[SPRITES_NUMER]) {

	if (isJudgeDisp) {

		++JudgeEffectCnt;

		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, 1.0 - JudgeEffectCnt * 0.05);

		//アニメーション
		if (CurrentTimeNotes - JudgeMakeTime < 0.05)  JudgeY = 73 + (CurrentTimeNotes - JudgeMakeTime) * 140;
		if (JudgeY >= 80) JudgeY = 80;

		switch (JudgeDispknd) {

		case PERFECT:			//良
			C2D_DrawImage(sprites[SPRITE_EFFECT_PERFECT].image, &sprites[SPRITE_EFFECT_PERFECT].params, &Tint);
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_PERFECT], 93, JudgeY);
			C2D_DrawImage(sprites[SPRITE_JUDGE_PERFECT].image, &sprites[SPRITE_JUDGE_PERFECT].params, NULL);
			break;

		case SPECIAL_PERFECT:	//特良
			C2D_DrawImage(sprites[SPRITE_EFFECT_SPECIAL_PERFECT].image, &sprites[SPRITE_EFFECT_SPECIAL_PERFECT].params, &Tint);
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_PERFECT], 93, JudgeY);
			C2D_DrawImage(sprites[SPRITE_JUDGE_PERFECT].image, &sprites[SPRITE_JUDGE_PERFECT].params, NULL);
			break;

		case NICE:				//可
			C2D_DrawImage(sprites[SPRITE_EFFECT_NICE].image, &sprites[SPRITE_EFFECT_NICE].params, &Tint);
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_NICE], 93, JudgeY);
			C2D_DrawImage(sprites[SPRITE_JUDGE_NICE].image, &sprites[SPRITE_JUDGE_NICE].params, NULL);
			break;

		case SPECIAL_NICE:		//特可
			C2D_DrawImage(sprites[SPRITE_EFFECT_SPECIAL_NICE].image, &sprites[SPRITE_EFFECT_SPECIAL_NICE].params, &Tint);
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_NICE], 93, JudgeY);
			C2D_DrawImage(sprites[SPRITE_JUDGE_NICE].image, &sprites[SPRITE_JUDGE_NICE].params, NULL);
			break;

		case BAD:				//不可
			C2D_SpriteSetPos(&sprites[SPRITE_JUDGE_BAD], 92, JudgeY);
			C2D_DrawImage(sprites[SPRITE_JUDGE_BAD].image, &sprites[SPRITE_JUDGE_BAD].params, NULL);
			break;

		}
		//snprintf(buf_notes, sizeof(buf_notes), "%f", JudgeY);
		//draw_debug(92, JudgeY, buf_notes);
		if (CurrentTimeNotes - JudgeMakeTime >= 0.5) isJudgeDisp = false;
	}

}

inline void notes_judge(double CurrentTimeNotes,int isDon,int isKatsu,int cnt,int branch) {

	OPTION_T Option;
	get_option(&Option);

	int CurrentJudgeNotes[2] = { -1,-1 };		//現在判定すべきノーツ ドン,カツ
	double CurrentJudgeNotesLag[2] = { -1,-1 };	//判定すべきノーツの誤差(s)

	JudgeRollState = -1;

	//連打の状態
	for (int i = 0, j = ROLL_MAX - 1; i < j; ++i) {

		if (RollNotes[i].flag &&
			Notes[RollNotes[i].start_id].judge_time < CurrentTimeNotes &&
			(RollNotes[i].end_id == -1 || (RollNotes[i].end_id != -1 && Notes[RollNotes[i].end_id].judge_time > CurrentTimeNotes))) JudgeRollState = RollNotes[i].knd;
	}

	//風船の処理
	int JudgeBalloonState = -1;
	for (int i = 0, j = BALLOON_MAX - 1; i < j; ++i) {

		if (BalloonNotes[i].flag && Notes[BalloonNotes[i].start_id].judge_time <= CurrentTimeNotes) {
			JudgeBalloonState = i;
			break;
		}
	}
	if (JBS != JudgeBalloonState && JudgeBalloonState != -1) {
		BalloonNotes[JudgeBalloonState].current_hit = 0;
		if (balloon[branch][BalloonCount[branch]] > 0) BalloonNotes[JudgeBalloonState].need_hit = balloon[branch][BalloonCount[branch]];
		else BalloonNotes[JudgeBalloonState].need_hit = 5;
		if (branch == 0) ++BalloonCount[0];
		else {
			++BalloonCount[1];
			++BalloonCount[2];
			++BalloonCount[3];
		}
	}
	JBS = JudgeBalloonState;

	if (Option.isAuto) {	//オート

		for (int i = 0, j = Notes.size() - 1; i < j; ++i) {

			if (Notes[i].flag && Notes[i].judge_time <= CurrentTimeNotes &&
				Notes[i].isThrough == false && Notes[i].knd < NOTES_ROLL) {

				switch (Notes[i].knd) {
				case NOTES_DON:
					play_sound(SOUND_DON);
					make_judge(PERFECT, CurrentTimeNotes);
					break;
				case NOTES_BIGDON:
					play_sound(SOUND_DON);
					make_judge(SPECIAL_PERFECT, CurrentTimeNotes);
					break;
				case NOTES_KATSU:
					play_sound(SOUND_KATSU);
					make_judge(PERFECT, CurrentTimeNotes);
					break;
				case NOTES_BIGKATSU:
					play_sound(SOUND_KATSU);
					make_judge(SPECIAL_PERFECT, CurrentTimeNotes);
					break;
				}
				if (Notes[i].knd == NOTES_BIGDON || Notes[i].knd == NOTES_BIGKATSU) update_score(SPECIAL_PERFECT);
				else if (Notes[i].knd == NOTES_DON || Notes[i].knd == NOTES_KATSU) update_score(PERFECT);
				delete_notes(i);
			}
		}

		if (JudgeRollState != -1) {	//連打

			if (cnt % AUTO_ROLL_FRAME == 0) {

				if (JudgeRollState == NOTES_ROLL) update_score(ROLL);
				else if (JudgeRollState == NOTES_BIGROLL) update_score(BIG_ROLL);

				play_sound(SOUND_DON);
			}
		}

		if (JudgeBalloonState != -1) {	//風船

			if (cnt % AUTO_ROLL_FRAME == 0) {

				play_sound(SOUND_DON);
				++BalloonNotes[JudgeBalloonState].current_hit;

				if (BalloonNotes[JudgeBalloonState].current_hit >= BalloonNotes[JudgeBalloonState].need_hit) {

					update_score(BALLOON_BREAK);	//破裂
					make_balloon_break();
				}
				else update_score(BALLOON);
			}
		}
	}

	else if (!Option.isAuto) {			//手動

		for (int j = 0,sd = 0,sk = 0,gn = ((isDon >= isKatsu) ? isDon : isKatsu); j < gn; ++j) {

			//判定すべきノーツを検索
			for (int i = 0, j = Notes.size(); i < j; ++i) {
	
				if (Notes[i].flag) {
	
					if (Notes[i].knd == NOTES_DON ||
						Notes[i].knd == NOTES_BIGDON ||
						Notes[i].knd == NOTES_BOMB) {	//ドン
	
						if (CurrentJudgeNotesLag[0] > fabs(Notes[i].judge_time - CurrentTimeNotes) ||
							CurrentJudgeNotesLag[0] == -1) {
	
							CurrentJudgeNotes[0] = i;
							CurrentJudgeNotesLag[0] = fabs(Notes[i].judge_time - CurrentTimeNotes);
						}
					}
					if (Notes[i].knd == NOTES_KATSU ||
						Notes[i].knd == NOTES_BIGKATSU ||
						Notes[i].knd == NOTES_BOMB) {	//カツ
	
						if (CurrentJudgeNotesLag[1] > fabs(Notes[i].judge_time - CurrentTimeNotes) ||
							CurrentJudgeNotesLag[1] == -1) {
	
							CurrentJudgeNotes[1] = i;
							CurrentJudgeNotesLag[1] = fabs(Notes[i].judge_time - CurrentTimeNotes);
						}
					}
				}
			}
	
			bool isBig,isBomb;
			if ((isDon > sd && Notes[CurrentJudgeNotes[0]].knd == NOTES_BIGDON) || (isKatsu > sk && Notes[CurrentJudgeNotes[1]].knd == NOTES_BIGKATSU)) isBig = true;
			else isBig = false;
			if ((isDon > sd && Notes[CurrentJudgeNotes[0]].knd == NOTES_BOMB) || (isKatsu > sk && Notes[CurrentJudgeNotes[1]].knd == NOTES_BOMB)) isBomb = true;
			else isBomb = false;
	
			if (isDon > sd && CurrentJudgeNotes[0] != -1) {	//ドン
	
				if (CurrentJudgeNotesLag[0] <= Option.judge_range_perfect) {			//良
					delete_notes(CurrentJudgeNotes[0]);
					if (isBig) {
						make_judge(SPECIAL_PERFECT, CurrentTimeNotes);
						update_score(SPECIAL_PERFECT);
					}
					else if (isBomb) {
						update_score(BAD);
					}
					else {
						make_judge(PERFECT, CurrentTimeNotes);
						update_score(PERFECT);
					}
				}
				else if (CurrentJudgeNotesLag[0] <= Option.judge_range_nice) {	//可
					delete_notes(CurrentJudgeNotes[0]);
					if (isBig) {
						make_judge(SPECIAL_NICE, CurrentTimeNotes);
						update_score(SPECIAL_NICE);
					}
					else if (isBomb) {
						update_score(BAD);
					}
					else {
						make_judge(NICE, CurrentTimeNotes);
						update_score(NICE);
					}
				}
				else if (CurrentJudgeNotesLag[0] <= Option.judge_range_bad) {	//不可
					make_judge(BAD, CurrentTimeNotes);
					delete_notes(CurrentJudgeNotes[0]);
					update_score(BAD);
				}
				++sd;
			}
	
			if (isKatsu > sk && CurrentJudgeNotes[1] != -1) {	//カツ
	
				if (CurrentJudgeNotesLag[1] <= Option.judge_range_perfect) {			//良
					delete_notes(CurrentJudgeNotes[1]);
					if (isBig) {
						make_judge(SPECIAL_PERFECT, CurrentTimeNotes);
						update_score(SPECIAL_PERFECT);
					}
					else if (isBomb) {
						update_score(BAD);
					}
					else {
						make_judge(PERFECT, CurrentTimeNotes);
						update_score(PERFECT);
					}
				}
				else if (CurrentJudgeNotesLag[1] <= Option.judge_range_nice) {	//可
					delete_notes(CurrentJudgeNotes[1]);
					if (isBig) {
						make_judge(SPECIAL_NICE, CurrentTimeNotes);
						update_score(SPECIAL_NICE);
					}
					else if (isBomb) {
						update_score(BAD);
					}
					else {
						make_judge(NICE, CurrentTimeNotes);
						update_score(NICE);
					}
				}
				else if (CurrentJudgeNotesLag[1] <= Option.judge_range_bad) {	//不可
					make_judge(BAD, CurrentTimeNotes);
					delete_notes(CurrentJudgeNotes[1]);
					update_score(BAD);
				}
				++sk;
			}
		}

		if (JudgeRollState != -1) {	//連打

			for (int i = 0, j = (isDon + isKatsu); i < j; ++i) {
				if (JudgeRollState == NOTES_ROLL) update_score(ROLL);
				else if (JudgeRollState == NOTES_BIGROLL) update_score(BIG_ROLL);
			}
		}

		int dc = 0;
		while (JudgeBalloonState != -1 && dc < isDon) {	//風船

			++BalloonNotes[JudgeBalloonState].current_hit;
			if (BalloonNotes[JudgeBalloonState].current_hit >= BalloonNotes[JudgeBalloonState].need_hit) {

				update_score(BALLOON_BREAK);	//破裂
				make_balloon_break();
				break;
			}
			else update_score(BALLOON);
			++dc;
		}
	}

	//風船の消去処理
	if (JudgeBalloonState != -1 && BalloonNotes[JudgeBalloonState].current_hit >= BalloonNotes[JudgeBalloonState].need_hit) {

		if (BalloonNotes[JudgeBalloonState].end_id != -1) delete_notes(BalloonNotes[JudgeBalloonState].end_id);
		else delete_notes(BalloonNotes[JudgeBalloonState].start_id);

		play_sound(SOUND_BALLOONBREAK);
		update_balloon_count(0);
	}
}

void notes_calc(int isDon, int isKatsu, double bpm, double CurrentTimeNotes, int cnt, C2D_Sprite sprites[SPRITES_NUMER], MEASURE_T Measure[MEASURE_MAX]) {

	OPTION_T Option;
	get_option(&Option);

	for (int i = 0, j = Notes.size() - 1; i < j; ++i) {	//計算

		if (Notes[i].flag) {

			Notes[i].x = Notes[i].x_ini - NOTES_AREA * Notes[i].scroll * (CurrentTimeNotes - Notes[i].pop_time) * (Notes[i].bpm / 240.0);

			switch (Notes[i].knd) {

			case NOTES_ROLL:
			case NOTES_BIGROLL:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag) {
					RollNotes[Notes[i].roll_id].start_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].start_id = i;
				}
				break;

			case NOTES_ROLLEND:
			case NOTES_BIGROLLEND:
				if (Notes[i].roll_id != -1 && RollNotes[Notes[i].roll_id].flag) {
					RollNotes[Notes[i].roll_id].end_x = Notes[i].x;
					RollNotes[Notes[i].roll_id].end_id = i;
				}
				break;

			case NOTES_BALLOON:
				if ((Notes[i].x <= NOTES_JUDGE_X && Notes[i].scroll > 0) || (Notes[i].x >= NOTES_JUDGE_X && Notes[i].scroll < 0)) Notes[i].x = NOTES_JUDGE_X;
				if (Notes[i].roll_id != -1) {
					BalloonNotes[Notes[i].roll_id].start_id = i;
				}
				break;

			case NOTES_BALLOONEND:
				if (Notes[i].roll_id != -1) {
					BalloonNotes[Notes[i].roll_id].end_id = i;
				}
				if (Notes[i].judge_time <= CurrentTimeNotes) {
					delete_notes(i);
				}
				break;

			case NOTES_DON:
			case NOTES_KATSU:
			case NOTES_BIGDON:
			case NOTES_BIGKATSU:
				if (CurrentTimeNotes - Notes[i].judge_time > (Option.judge_range_bad) && !Notes[i].isThrough) {
					update_score(THROUGH);
					Notes[i].isThrough = true;
				}
				break;
			case NOTES_BOMB:
				if (CurrentTimeNotes - Notes[i].judge_time > (Option.judge_range_bad) && !Notes[i].isThrough) {
					Notes[i].isThrough = true;
				}
				break;
			}
		}
	}

	for (int i = 0, j = Notes.size() - 1; i < j; ++i) {	//連打のバグ回避のためノーツの削除は一番最後

		if (Notes[i].flag &&
			((Notes[i].x <= 20 && Notes[i].scroll > 0) || (Notes[i].x >= 420 && Notes[i].scroll < 0)) &&
			Notes[i].knd != NOTES_ROLL && Notes[i].knd != NOTES_BIGROLL) {

			if (Notes[i].isThrough == false && Notes[i].knd < NOTES_ROLL) {

				if (!Option.isAuto) {
					update_score(THROUGH);
					Notes[i].isThrough = true;
				}
				else {	//オート時はスルー以外良判定に
					if (Notes[i].knd == NOTES_DON || Notes[i].knd == NOTES_KATSU) update_score(PERFECT);
					else if (Notes[i].knd == NOTES_BIGDON || Notes[i].knd == NOTES_BIGKATSU) update_score(SPECIAL_PERFECT);
					if (Notes[i].knd == NOTES_DON || Notes[i].knd == NOTES_BIGDON) play_sound(SOUND_DON);
					if (Notes[i].knd == NOTES_KATSU || Notes[i].knd == NOTES_BIGKATSU) play_sound(SOUND_KATSU);
				}
			}
			delete_notes(i);
		}
	}
	notes_judge(CurrentTimeNotes, isDon, isKatsu, cnt, ((Measure[MeasureCount].branch == -1) ? 0 : Measure[MeasureCount].branch - 11));
}

inline void notes_draw(C2D_Sprite sprites[SPRITES_NUMER]) {

	int notes_y = 109;

	for (int i = 0, j = Notes.size(); i < j; ++i) {	//描画

		if (Notes[i].flag) {

			draw_lyric_text(Text[get_lang()][TEXT_NONE + Notes[i].text_id], Notes[i].x, 132, 0.4);
			switch (Notes[i].knd) {
			case NOTES_DON:
				sprites[SPRITE_DON].params.pos.x = Notes[i].x;
				sprites[SPRITE_DON].params.pos.y = notes_y;
				C2D_DrawImage(sprites[SPRITE_DON].image, &sprites[SPRITE_DON].params, NULL);
				break;
			case NOTES_KATSU:
				sprites[SPRITE_KATSU].params.pos.x = Notes[i].x;
				sprites[SPRITE_KATSU].params.pos.y = notes_y;
				C2D_DrawImage(sprites[SPRITE_KATSU].image, &sprites[SPRITE_KATSU].params, NULL);
				break;
			case NOTES_BIGDON:
				sprites[SPRITE_BIG_DON].params.pos.x = Notes[i].x;
				sprites[SPRITE_BIG_DON].params.pos.y = notes_y;
				C2D_DrawImage(sprites[SPRITE_BIG_DON].image, &sprites[SPRITE_BIG_DON].params, NULL);
				break;
			case NOTES_BIGKATSU:
				sprites[SPRITE_BIG_KATSU].params.pos.x = Notes[i].x;
				sprites[SPRITE_BIG_KATSU].params.pos.y = notes_y;
				C2D_DrawImage(sprites[SPRITE_BIG_KATSU].image, &sprites[SPRITE_BIG_KATSU].params, NULL);
				break;
			case NOTES_ROLL:

				if (RollNotes[Notes[i].roll_id].flag) {

					double end_x;
					if (RollNotes[Notes[i].roll_id].end_id == -1 || RollNotes[Notes[i].roll_id].end_x >= 420.0f) end_x = TOP_WIDTH + 20.0f;
					else end_x = RollNotes[Notes[i].roll_id].end_x;

					if (Notes[i].scroll > 0) {
						for (int n = 0, m = (end_x - RollNotes[Notes[i].roll_id].start_x) / 8.0; n < m; n++) {
							sprites[SPRITE_ROLL_INT].params.pos.x = (int)Notes[i].x + 8 * n;
							sprites[SPRITE_ROLL_INT].params.pos.y = notes_y;
							if (sprites[SPRITE_ROLL_INT].params.pos.x > 0) C2D_DrawImage(sprites[SPRITE_ROLL_INT].image, &sprites[SPRITE_ROLL_INT].params, NULL);
							if (n > 1 && sprites[SPRITE_ROLL_INT].params.pos.x > 0) draw_lyric_text(Text[get_lang()][TEXT_ROLLINT], sprites[SPRITE_ROLL_INT].params.pos.x, 132, 0.4);
						}
						sprites[SPRITE_ROLL_START].params.pos.x = Notes[i].x;
						sprites[SPRITE_ROLL_START].params.pos.y = notes_y;
						if (Notes[i].x > 0) C2D_DrawImage(sprites[SPRITE_ROLL_START].image, &sprites[SPRITE_ROLL_START].params, NULL);
					}
					else if (Notes[i].scroll < 0) {
						for (int n = 0, m = (RollNotes[Notes[i].roll_id].start_x - end_x) / 8.0; n < m; n++) {
							sprites[SPRITE_ROLL_INT].params.pos.x = (int)Notes[i].x + 8 * (n * -1);
							sprites[SPRITE_ROLL_INT].params.pos.y = notes_y;
							if (sprites[SPRITE_ROLL_INT].params.pos.x < 420) C2D_DrawImage(sprites[SPRITE_ROLL_INT].image, &sprites[SPRITE_ROLL_INT].params, NULL);
							if (n > 1 && sprites[SPRITE_ROLL_INT].params.pos.x < 420) draw_lyric_text(Text[get_lang()][TEXT_ROLLINT], sprites[SPRITE_ROLL_INT].params.pos.x, 132, 0.4);
						}
						sprites[SPRITE_ROLL_START].params.pos.x = Notes[i].x;
						sprites[SPRITE_ROLL_START].params.pos.y = notes_y;
						if (Notes[i].x < 420) C2D_DrawImage(sprites[SPRITE_ROLL_START].image, &sprites[SPRITE_ROLL_START].params, NULL);
					}
				}
				break;

			case NOTES_BIGROLL:

				if (RollNotes[Notes[i].roll_id].flag) {

					double end_x;
					if (RollNotes[Notes[i].roll_id].end_id == -1 || RollNotes[Notes[i].roll_id].end_x >= 420.0f) end_x = TOP_WIDTH + 20.0f;
					else end_x = RollNotes[Notes[i].roll_id].end_x;

					if (Notes[i].scroll > 0) {
						for (int n = 0, m = (end_x - RollNotes[Notes[i].roll_id].start_x) / 8.0; n < m; n++) {
							sprites[SPRITE_BIG_ROLL_INT].params.pos.x = (int)Notes[i].x + 8 * n;
							sprites[SPRITE_BIG_ROLL_INT].params.pos.y = notes_y;
							if (sprites[SPRITE_BIG_ROLL_INT].params.pos.x > 0) C2D_DrawImage(sprites[SPRITE_BIG_ROLL_INT].image, &sprites[SPRITE_BIG_ROLL_INT].params, NULL);
							if (n > 2 && sprites[SPRITE_BIG_ROLL_INT].params.pos.x > 0) draw_lyric_text(Text[get_lang()][TEXT_ROLLINT], sprites[SPRITE_BIG_ROLL_INT].params.pos.x, 132, 0.4);
						}
						sprites[SPRITE_BIG_ROLL_START].params.pos.x = Notes[i].x;
						sprites[SPRITE_BIG_ROLL_START].params.pos.y = notes_y;
						if (Notes[i].x > 0) C2D_DrawImage(sprites[SPRITE_BIG_ROLL_START].image, &sprites[SPRITE_BIG_ROLL_START].params, NULL);
					}
					else if (Notes[i].scroll < 0) {
						for (int n = 0, m = (RollNotes[Notes[i].roll_id].start_x - end_x) / 8.0; n < m; n++) {
							sprites[SPRITE_BIG_ROLL_INT].params.pos.x = (int)Notes[i].x + 8 * (n * -1);
							sprites[SPRITE_BIG_ROLL_INT].params.pos.y = notes_y;
							if (sprites[SPRITE_BIG_ROLL_INT].params.pos.x < 420) C2D_DrawImage(sprites[SPRITE_BIG_ROLL_INT].image, &sprites[SPRITE_BIG_ROLL_INT].params, NULL);
							if (n > 2 && sprites[SPRITE_BIG_ROLL_INT].params.pos.x < 420) draw_lyric_text(Text[get_lang()][TEXT_ROLLINT], sprites[SPRITE_BIG_ROLL_INT].params.pos.x, 132, 0.4);
						}
						sprites[SPRITE_BIG_ROLL_START].params.pos.x = Notes[i].x;
						sprites[SPRITE_BIG_ROLL_START].params.pos.y = notes_y;
						if (Notes[i].x < 420) C2D_DrawImage(sprites[SPRITE_BIG_ROLL_START].image, &sprites[SPRITE_BIG_ROLL_START].params, NULL);
					}
					break;
				}

			case NOTES_BALLOON:

				if (BalloonNotes[Notes[i].roll_id].current_hit == 0) {

					sprites[SPRITE_BALLOON].params.pos.x = Notes[i].x;
					sprites[SPRITE_BALLOON].params.pos.y = notes_y;
					C2D_DrawImage(sprites[SPRITE_BALLOON].image, &sprites[SPRITE_BALLOON].params, NULL);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 0.2f) {

					sprites[SPRITE_BALLOON_1].params.pos.x = NOTES_JUDGE_X;
					sprites[SPRITE_BALLOON_1].params.pos.y = notes_y;
					C2D_DrawImage(sprites[SPRITE_BALLOON_1].image, &sprites[SPRITE_BALLOON_1].params, NULL);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 0.4f) {

					sprites[SPRITE_BALLOON_2].params.pos.x = NOTES_JUDGE_X;
					sprites[SPRITE_BALLOON_2].params.pos.y = notes_y;
					C2D_DrawImage(sprites[SPRITE_BALLOON_2].image, &sprites[SPRITE_BALLOON_2].params, NULL);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 0.6f) {

					sprites[SPRITE_BALLOON_3].params.pos.x = NOTES_JUDGE_X;
					sprites[SPRITE_BALLOON_3].params.pos.y = notes_y;
					C2D_DrawImage(sprites[SPRITE_BALLOON_3].image, &sprites[SPRITE_BALLOON_3].params, NULL);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit * 0.8f) {

					sprites[SPRITE_BALLOON_4].params.pos.x = NOTES_JUDGE_X;
					sprites[SPRITE_BALLOON_4].params.pos.y = notes_y;
					C2D_DrawImage(sprites[SPRITE_BALLOON_4].image, &sprites[SPRITE_BALLOON_4].params, NULL);
				}
				else if (BalloonNotes[Notes[i].roll_id].current_hit <= BalloonNotes[Notes[i].roll_id].need_hit) {

					sprites[SPRITE_BALLOON_5].params.pos.x = NOTES_JUDGE_X;
					sprites[SPRITE_BALLOON_5].params.pos.y = notes_y;
					C2D_DrawImage(sprites[SPRITE_BALLOON_5].image, &sprites[SPRITE_BALLOON_5].params, NULL);
				}
				if (BalloonNotes[Notes[i].roll_id].current_hit >= 1) update_balloon_count(BalloonNotes[Notes[i].roll_id].need_hit - BalloonNotes[Notes[i].roll_id].current_hit);
				break;
			case NOTES_ROLLEND:
				sprites[SPRITE_ROLL_END].params.pos.x = Notes[i].x;
				sprites[SPRITE_ROLL_END].params.pos.y = notes_y;
				C2D_SpriteSetScale(&sprites[SPRITE_ROLL_END], sign(Notes[i].scroll), 1);
				C2D_DrawImage(sprites[SPRITE_ROLL_END].image, &sprites[SPRITE_ROLL_END].params, NULL);
				break;
			case NOTES_BIGROLLEND:
				sprites[SPRITE_BIG_ROLL_END].params.pos.x = Notes[i].x;
				sprites[SPRITE_BIG_ROLL_END].params.pos.y = notes_y;
				C2D_SpriteSetScale(&sprites[SPRITE_BIG_ROLL_END], sign(Notes[i].scroll), 1);
				C2D_DrawImage(sprites[SPRITE_BIG_ROLL_END].image, &sprites[SPRITE_BIG_ROLL_END].params, NULL);
				break;
			case NOTES_BOMB:
				sprites[SPRITE_BOMB].params.pos.x = Notes[i].x;
				sprites[SPRITE_BOMB].params.pos.y = notes_y;
				C2D_DrawImage(sprites[SPRITE_BOMB].image, &sprites[SPRITE_BOMB].params, NULL);
				break;
			}
		}
	}

	//割れた風船
	if (isBalloonBreakDisp) {
		BalloonBreakCount--;
		C2D_ImageTint Tint;
		C2D_AlphaImageTint(&Tint, BalloonBreakCount / 40.0);
		C2D_SpriteSetPos(&sprites[SPRITE_BALLOON_6], NOTES_JUDGE_X, notes_y);
		C2D_DrawImage(sprites[SPRITE_BALLOON_6].image, &sprites[SPRITE_BALLOON_6].params, &Tint);
	}
	if (BalloonBreakCount <= 0) isBalloonBreakDisp = false;
}

int get_branch_course() {

	return Branch.course;
}

int ctoi(char c) {

	switch (c) {
	case '1':
	case 'D':
	case 'E':
	case 'F':
		return NOTES_DON;
	case '2':
	case 'G':
	case 'H':
		return NOTES_KATSU;
	case '3':
	case 'A':
	case 'I':
		return NOTES_BIGDON;
	case '4':
	case 'B':
	case 'J':
		return NOTES_BIGKATSU;
	case '5':
	case 'K':
		return NOTES_ROLL;
	case '6':
	case 'L':
		return NOTES_BIGROLL;
	case '7':
	case '9':
	case 'M':
		return NOTES_BALLOON;
	case '8':
	case 'N':
		return NOTES_ROLLEND;
	case 'C': return NOTES_BOMB;
	default: return 0;
	}
}

int notes_cmp(const void *p, const void *q) {	//比較用

	int pp = ((NOTES_T*)p)->judge_time * 10000;
	int qq = ((NOTES_T*)q)->judge_time * 10000;

	//if (((NOTES_T*)p)->flag == false) pp = INT_MAX;
	//if (((NOTES_T*)p)->flag == false) qq = INT_MAX;

	return qq - pp;
}

void notes_sort() {	//ノーツを出現順にソート
	qsort(Notes.data(), Notes.size(), sizeof(NOTES_T), notes_cmp);
}

void delete_roll(int i) {

	if (i >= 0 && i < ROLL_MAX) {
		if (RollNotes[i].start_id != -1 && Notes[RollNotes[i].start_id].flag) delete_notes(RollNotes[i].start_id);
		RollNotes[i].id = -1;
		RollNotes[i].start_x = -1;
		RollNotes[i].start_id = -1;
		RollNotes[i].end_x = -1;
		RollNotes[i].end_id = -1;
		RollNotes[i].flag = false;
		RollNotes[i].knd = -1;
	}
}

inline void init_roll__notes() {

	for (int i = 0, j = ROLL_MAX - 1; i < j; ++i) {
		delete_roll(i);
	}
}

inline int find_roll_id() {

	for (int i = 0, j = ROLL_MAX - 1; i < j; ++i) {
		if (!RollNotes[i].flag) return i;
	}
	return -1;
}

inline int make_roll_start(int NotesId) {

	int id = find_roll_id();
	if (id != -1) {

		RollNotes[id].id = id;
		RollNotes[id].start_x = Notes[NotesId].x;
		RollNotes[id].knd = Notes[NotesId].knd;
		RollNotes[id].end_x = -1;
		RollNotes[id].flag = true;
		return id;
	}
	else return -1;
}

static int find_roll_end_id() {	//startの値だけ入ってる連打idを返す

	for (int i = 0, j = ROLL_MAX - 1; i < j; ++i) {

		if (RollNotes[i].flag &&
			RollNotes[i].start_x != -1 &&
			RollNotes[i].end_x == -1) return i;
	}
	return -1;
}

inline int make_roll_end(int NotesId) {

	int id = find_roll_end_id();
	if (id != -1) {

		RollNotes[id].end_x = Notes[NotesId].x;
		return id;
	}
	else return -1;
}

void make_balloon_break() {

	isBalloonBreakDisp = true;
	BalloonBreakCount = 40;
}

void delete_balloon(int i) {

	if (i >= 0 && i < BALLOON_MAX) {
		BalloonNotes[i].id = -1;
		BalloonNotes[i].start_id = -1;
		BalloonNotes[i].end_id = -1;
		BalloonNotes[i].flag = false;
		BalloonNotes[i].current_hit = 0;
		BalloonNotes[i].need_hit = 5;
	}
}

inline void init_balloon_notes() {

	for (int i = 0; i < BALLOON_MAX - 1; ++i) {
		delete_balloon(i);
	}
}

inline int find_balloon_id() {

	for (int i = 0, j = BALLOON_MAX - 1; i < j; ++i) {
		if (!BalloonNotes[i].flag) return i;
	}
	return -1;
}

int make_balloon_start(int NotesId) {

	int id = find_balloon_id();
	if (id != -1) {

		BalloonNotes[id].id = id;
		BalloonNotes[id].start_id = NotesId;
		BalloonNotes[id].end_id = -1;
		BalloonNotes[id].flag = true;
		return id;
	}
	else return -1;
}

int find_balloon_end_id() {	//startの値だけ入ってる風船idを返す

	for (int i = 0, j = BALLOON_MAX - 1; i < j; ++i) {

		if (BalloonNotes[i].flag &&
			BalloonNotes[i].start_id != -1 &&
			BalloonNotes[i].end_id == -1) return i;
	}
	return -1;
}

int make_balloon_end(int NotesId) {

	int id = find_balloon_end_id();
	if (id != -1) return id;
	else return -1;
}

void delete_notes(int i) {

	if (i >= 0 &&
		Notes[i].roll_id != -1 &&
		(Notes[i].knd == NOTES_ROLLEND || Notes[i].knd == NOTES_BIGROLLEND) &&
		RollNotes[Notes[i].roll_id].flag == true
		) {	//連打削除

		delete_notes(RollNotes[Notes[i].roll_id].start_id);
		delete_roll(Notes[i].roll_id);
		update_score(ROLL_END);

	}

	if (i >= 0 &&
		Notes[i].roll_id != -1 &&
		BalloonNotes[Notes[i].roll_id].flag) {					//風船削除

		if (Notes[i].knd == NOTES_BALLOONEND) {

			if (BalloonNotes[Notes[i].roll_id].start_id != -1) delete_notes(BalloonNotes[Notes[i].roll_id].start_id);
			delete_balloon(Notes[i].roll_id);
			update_balloon_count(0);
		}
		else if (Notes[i].knd == NOTES_BALLOON) {

			BalloonNotes[Notes[i].roll_id].start_id = -1;

			if (BalloonNotes[Notes[i].roll_id].end_id == -1) {

				delete_balloon(Notes[i].roll_id);
			}
		}
	}

	if (i >= 0 && i < (int)Notes.size()) {
		Notes[i].flag = false;
		Notes[i].num = 0;
		Notes[i].knd = 0;
		Notes[i].notes_max = 0;
		Notes[i].x_ini = 0;
		Notes[i].x = 0;
		Notes[i].create_time = 0;
		Notes[i].judge_time = 0;
		Notes[i].pop_time = 0;
		Notes[i].bpm = 0;
		Notes[i].scroll = 0;
		Notes[i].roll_id = -1;
		Notes[i].isThrough = false;
		Notes[i].text_id = 0;
	}
}
bool get_notes_finish() {

	if (isNotesLoad) return false;
	for (int i = 0, j = Notes.size() - 1; i < j; ++i) {

		if (Notes[i].flag && !Notes[i].isThrough) return false;
	}
	return true;
}

C2D_TextBuf g_NotesText = C2D_TextBufNew(4096);
C2D_Text NotesText;

void draw_notes_text(float x, float y, const char *text, float *width, float *height) {

	float size = 0.6;
	C2D_TextBufClear(g_NotesText);
	C2D_TextFontParse(&NotesText, font, g_NotesText, text);
	C2D_TextOptimize(&NotesText);
	C2D_DrawText(&NotesText, C2D_WithColor | C2D_AlignRight, x, y, 1.0f, size, size, C2D_Color32f(black, black, black, 1.0f));
}

inline void draw_lyric_text(const char *text, float x, float y, float size) {

	if (TextCnt < 300) {
		C2D_TextBufClear(g_NotesText);
		C2D_TextParse(&NotesText, g_NotesText, text);
		C2D_TextOptimize(&NotesText);
		C2D_DrawText(&NotesText, C2D_WithColor | C2D_AlignCenter, x, y, 0.0f, size, size, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
		++TextCnt;
	}
}

void draw_condition_text(float x, float y, const char *text, float *width, float *height) {

	float size = 0.55;
	C2D_TextBufClear(g_NotesText);
	C2D_TextParse(&NotesText, g_NotesText, text);
	C2D_TextOptimize(&NotesText);
	C2D_TextGetDimensions(&NotesText, size, size, width, height);
	C2D_DrawText(&NotesText, C2D_WithColor, x, y, 1.0f, size, size, C2D_Color32f(black, black, black, 1.0f));
}

void draw_title() {

	TJA_HEADER_T Header;
	get_tja_header(&Header);
	float width = 0, height = 0;

	if (Header.subtitle_state != -1 && Header.subtitle_state != 1) draw_notes_text(TOP_WIDTH, 20, Header.subtitle, &width, &height);
	draw_notes_text(TOP_WIDTH - 5, 5, Header.title, &width, &height);
}

void draw_condition() {

	OPTION_T Option;
	get_option(&Option);
	extern char *exam[4][4];
	float width = 0, height = 0, tx = 0;

	for (int j = 0; j < 4; ++j) {

		tx = 0;
		if (strcmp(exam[j][0], "jb") == 0) draw_condition_text(50, 148+20*j, Text[get_lang()][TEXT_NUM_BAD], &width, &height);
		else if (strcmp(exam[j][0], "jg") == 0) draw_condition_text(50, 148+20*j, Text[get_lang()][TEXT_NUM_NICE], &width, &height);
		else if (strcmp(exam[j][0], "jp") == 0) draw_condition_text(50, 148+20*j, Text[get_lang()][TEXT_NUM_PERFECT], &width, &height);
		else if (strcmp(exam[j][0], "s") == 0) draw_condition_text(50, 148+20*j, Text[get_lang()][TEXT_NUM_SCORE], &width, &height);
		else if (strcmp(exam[j][0], "r") == 0) draw_condition_text(50, 148+20*j, Text[get_lang()][TEXT_NUM_ROLL], &width, &height);
		else if (strcmp(exam[j][0], "h") == 0) draw_condition_text(50, 148+20*j, Text[get_lang()][TEXT_NUM_HIT], &width, &height);
		else if (strcmp(exam[j][0], "g") == 0) draw_condition_text(50, 148+20*j, Text[get_lang()][TEXT_NUM_GAUGE], &width, &height);
		tx += width;
		draw_condition_text(50+tx, 148+20*j, exam[j][1], &width, &height);
		tx += width;

		if (strcmp(exam[j][0], "g") == 0) {
			draw_condition_text(50+tx, 148+20*j, "%", &width, &height);
			tx += width;
		}
		if (strcmp(exam[j][3], "m") == 0) draw_condition_text(50+tx, 148+20*j, Text[get_lang()][TEXT_NUM_UP], &width, &height);
		else if (strcmp(exam[j][3], "l") == 0) draw_condition_text(50+tx, 148+20*j, Text[get_lang()][TEXT_NUM_DOWN], &width, &height);
	}
}
inline void init_notes_structure() {

	for (int i = 0, j = Notes.size() - 1; i < j; i += 4) {
		delete_notes(i);
		delete_notes(i+1);
		delete_notes(i+2);
		delete_notes(i+3);
	}
}

void init_notes(TJA_HEADER_T TJA_Header) {

	OPTION_T Option;
	get_option(&Option);

	init_notes_structure();
	init_roll__notes();
	init_balloon_notes();
	Command.data[0] = 0; Command.data[1] = 0; Command.data[2] = 0;
	Command.knd = 0; Command.val[0] = 0; Command.val[1] = 0; Command.val[2] = 0;
	bnc = 512, nc = 0, bid = 0, id = -1, TJAVER = TJA_Header.ver;
	bpm = TJA_Header.bpm, offset = TJA_Header.offset + Option.offset, NowBPM = bpm;
	for (int i = 0, j = (int)(sizeof(balloon[0]) / sizeof(balloon[0][0])); i < j; ++i) {
		balloon[0][i] = TJA_Header.balloon[0][i];
		balloon[1][i] = TJA_Header.balloon[1][i];
		balloon[2][i] = TJA_Header.balloon[2][i];
		balloon[3][i] = TJA_Header.balloon[3][i];
	}
	NotesNumber = 0, NotesCount = 0, NowMeCount = 0, RollState = 0, MeasureCount = 0;
	MinMeasureCount = ((Option.measure > 0) ? stme : -1);
	MaxMeasureCount = 0, isNotesLoad = true, isJudgeDisp = false;
	JudgeMakeTime = 0, JudgeDispknd = -1, JudgeY = 70, JudgeRollState = -1;
	//isAuto = true;	//要変更
	BalloonCount[0] = 0, BalloonCount[1] = 0, BalloonCount[2] = 0, BalloonCount[3] = 0;
	BalloonBreakCount = 0;
	isBalloonBreakDisp = false;
	isGOGOTime = false;
	Branch.knd = 0,Branch.x = 0,Branch.y = 0,Branch.course = -1,Branch.next = false,Branch.wait = false;
	isLevelHold = false;
	PreNotesKnd = 0, TotalFailedCount = 0, dcd = 0, TextCnt = 0;
	for (int i = 0, j = BARLINE_MAX - 1; i < j; ++i) {
		BarLine[i].flag = false;
		BarLine[i].scroll = 0;
		BarLine[i].measure = 0;
		BarLine[i].x_ini = 0;
		BarLine[i].create_time = 0;
		BarLine[i].isDisp = false;
	}
	Notes.clear();
	Notes.resize(64);
}
int sign(double A) {	//正か負かの判別
	return (A > 0) - (A < 0);
}
void newfont() {
	font = C2D_FontLoad("romfs:/gfx/main.bcfnt");
	Notes.reserve(2048);
	Notes.resize(64);
}
void fontfree() {
	C2D_TextBufDelete(g_NotesText);
	C2D_FontFree(font);
	Notes.clear();
	std::vector<NOTES_T>().swap(Notes);
}
