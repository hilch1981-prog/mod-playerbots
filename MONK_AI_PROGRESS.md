# Legends MoP 5.4.8 Monk AI Progress

Target upstream: `Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8` (`master`)

Working branch: `feature/monk-ai-object-context`

> Note: the connected GitHub tool cannot create a new GitHub fork. This branch is being used as a resumable staging/work branch. The implementation is stored under `contrib/legends-monk-ai/` so it does not pollute the active Chipa `mop-5.4.8-v2` source layout. Move/apply the overlay to a true Legends fork before build/PR validation.

## 진행 상황 (최종 갱신: 2026-09-06)
- [x] MonkAiObjectContext 클래스 뼈대 생성
- [x] MonkStrategyFactory 등록 (`tank`, `heal`, `melee`, `aoe`, `melee aoe`, `cure`, `nc`)
- [x] 양조(Brewmaster) 전략 구현
- [x] 운무(Mistweaver) 전략 구현
- [x] 운기(Windwalker) 전략 구현
- [x] 스펙 판별 로직 확인 (`PlayerbotSpec.cpp`에 Brewmaster/Windwalker/Mistweaver 및 tank/heal 판별이 이미 구현되어 있어 신규 수정 불필요)
- [x] 정적 factory/action/trigger 이름 대조
- [ ] 빌드 검증 (`PLAYERBOTS=1`) — 실행 환경의 GitHub DNS 차단으로 원본 checkout 단계에서 중단
- [ ] 인게임 테스트 (파티 합류, 로테이션 동작, 힐 동작)

## 이번 실행 사전/종료 확인
- Upstream Monk/Monk AI/MonkAiObjectContext 중복 PR: 확인되지 않음
- 관련 open issue: #413(일반 bot 명령/npcbot), #150(Tushui Monk NPC) — 본 PlayerBot Monk AI 구현과 직접 중복되지 않음
- Upstream PR #389(Playerbot): merged 상태이며 현재 mod_playerbots 기반 코드의 출처로 확인
- 최종 재확인 upstream `master`: `3ec151e16c7912b217838040ac1bb30c6f1fc84d` (`Modernize TreatManager (#423)`)
- 작업 도중 동일 Monk 구현 PR 신규 생성: 없음
- Upstream write 권한: 없음 (read-only)
- 동일 Legends 저장소의 `hilch1981-prog` fork: 없음
- 치파팩 `MOP_V2_Repack/playerbot-v2-poc`는 `hilch1981-prog/mod-playerbots@mop-5.4.8-v2`를 submodule로 사용

## 이번 실행 커밋
- `862fa1ef9d0aa2e733bb115d29b4a44a3feb6a3a` — 진행 상태 추적 파일 생성
- `74c94d781c28bb180fddc9471c38b39fb3581a36` — Legends용 Monk AI 3스펙 Strategy/Trigger/Action overlay + AiFactory 활성화 패치
- `d10c56cfd44c49c7ce6c7752ed2cc447519fa54b` — Monk trigger 매크로 include 누락 수정
- `40b7bd4018b0270cf7cc7f90fdd2dc51aa87bcb5` — Monk 비전투 역할(tank/dps/cure) 누락 보강 패치
- `b9b7e228a5d5fc2559655c1b0cf5104876f97d08` — 진행 상태 갱신
- `0af1b0df6898ea69e143d384932ea0b1bf60455c` — 5.4.8 spell validation/적용 절차 문서 보강

## 발견된 버그 / 이슈
- [BLOCKER] 현재 연결된 GitHub API에는 fork 생성 동작이 없어 `Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8`의 새 fork를 자동 생성할 수 없음. 구현은 staging branch에서 계속하고, upstream PR 전에 정식 fork로 이식 필요.
- [INFO] 현재 `mop-5.4.8-v2` 브랜치는 Legends PR #389의 `src/strategy` 레이아웃과 다른 신형 mod-playerbots 레이아웃이므로 Legends용 코드를 기존 `src/Ai`에 임의로 섞지 않음.
- [FIXED] `MonkTriggers.h`가 `BUFF_TRIGGER`/`CURE_TRIGGER` 등 매크로를 확실히 제공받도록 `Playerbots.h` 명시 include 추가 — `d10c56c`.
- [FIXED] upstream `AiFactory::AddDefaultNonCombatStrategies`에 `CLASS_MONK` 분기가 없어 Brewmaster도 기본 `dps assist`로 떨어지는 문제를 발견. Brewmaster=`tank assist`, Windwalker=`dps assist`, Mistweaver=`dps assist + cure` 패치 작성 — `40b7bd4`.
- [ENV BLOCKER] 로컬 컴파일 검증을 위해 upstream checkout을 시도했으나 실행 컨테이너에서 `Could not resolve host: github.com`으로 실패. 이는 C++ 빌드 실패가 아니라 현재 실행 환경 네트워크 차단이며 빌드 PASS로 처리하지 않음.
- [PENDING BUILD] overlay는 의도적으로 live `mop-5.4.8-v2` 빌드 경로 밖에 있어 이 staging branch 자체 CI로 Legends 컴파일을 검증할 수 없음.

## 5.4.8 / build 18414 검증 메모
- Stance of the Wise Serpent = 115070 확인
- Expel Harm = 115072 확인
- Fortifying Brew = 115203 확인
- Provoke = 115546 확인
- Spear Hand Strike = 116705 확인
- Touch of Karma = 122470 확인
- Detox = 115450 확인
- Mana Tea = 115294 확인
- Guard = 115295 확인
- Elusive Brew = 115308 확인
- Moderate Stagger aura = 124274, Heavy Stagger aura = 124273 확인
- 실제 캐스트는 Legends 기존 패턴대로 DBC spell name 기반으로 작성; donor 구현 코드는 복사하지 않음

## 참고 소스
- 스펠 우선순위 로직 참고: DigiD702/mod-playerbots `BotRotation_Monk.cpp`, `BotRotation_Healers.cpp` (Mistweaver 부분) — 코드 그대로 복붙 금지, 로직만 참고해서 Legends 저장소의 Strategy/Trigger/Action 문법으로 재작성
- 구조 템플릿: Legends `modules/mod_playerbots/src/strategy/Classes/warrior/`, `paladin/`, `shaman/`
- 대상 클라이언트: MoP 5.4.8 build 18414

## 다음 작업
1. 매 실행 시작 시 upstream master / Monk 관련 PR·Issue 재확인
2. true Legends fork가 준비되면 overlay를 실제 `modules/mod_playerbots/src/strategy/Classes/monk/`로 이식하고 두 AiFactory patch 적용
3. `PLAYERBOTS=1` 빌드 → 오류를 별도 fix commit으로 수정
4. Brewmaster/Mistweaver/Windwalker 인게임 회귀 테스트
5. 3스펙 + 빌드 PASS 전에는 upstream Issue/PR 생성하지 않음
