# Legends MoP 5.4.8 Monk AI Progress

Target upstream: `Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8` (`master`)

Working branch: `feature/monk-ai-object-context`

> Note: the connected GitHub tool cannot create a new GitHub fork. This branch is being used as a resumable staging/work branch. The implementation must remain compatible with the Legends `modules/mod_playerbots/src/strategy/Classes/*` architecture and be moved to a true Legends fork before the upstream PR.

## 진행 상황 (최종 갱신: 2026-09-06)
- [ ] MonkAiObjectContext 클래스 뼈대 생성
- [ ] MonkStrategyFactory 등록
- [ ] 양조(Brewmaster) 전략 구현
- [ ] 운무(Mistweaver) 전략 구현
- [ ] 운기(Windwalker) 전략 구현
- [ ] 스펙 판별 로직 (PlayerbotSpec.cpp 연동)
- [ ] 빌드 검증
- [ ] 인게임 테스트 (파티 합류, 로테이션 동작, 힐 동작)

## 이번 실행 사전 확인
- Upstream Monk/Monk AI/MonkAiObjectContext 중복 PR: 확인되지 않음
- 관련 open issue: #413(일반 bot 명령/npcbot), #150(Tushui Monk NPC) — 본 PlayerBot Monk AI 구현과 직접 중복되지 않음
- Upstream PR #389(Playerbot): merged 상태이며 현재 mod_playerbots 기반 코드의 출처로 확인
- Upstream write 권한: 없음 (read-only)
- 동일 Legends 저장소의 `hilch1981-prog` fork: 없음
- 치파팩 `MOP_V2_Repack/playerbot-v2-poc`는 `hilch1981-prog/mod-playerbots@mop-5.4.8-v2`를 submodule로 사용

## 발견된 버그 / 이슈
- [BLOCKER] 현재 연결된 GitHub API에는 fork 생성 동작이 없어 `Legends-of-Azeroth/Legends-of-Azeroth-Pandaria-5.4.8`의 새 fork를 자동 생성할 수 없음. 구현은 이 staging branch에서 계속하고, upstream PR 전에 정식 fork로 이식 필요.
- [INFO] 현재 `mop-5.4.8-v2` 브랜치는 Legends PR #389의 `src/strategy` 레이아웃과 다른 신형 mod-playerbots 레이아웃이므로 Legends용 코드를 기존 `src/Ai`에 임의로 섞지 않음.

## 참고 소스
- 스펠 우선순위 로직 참고: DigiD702/mod-playerbots `BotRotation_Monk.cpp`, `BotRotation_Healers.cpp` (Mistweaver 부분) — 코드 그대로 복붙 금지, 로직만 참고해서 Legends 저장소의 Strategy/Trigger/Action 문법으로 재작성
- 구조 템플릿: Legends `modules/mod_playerbots/src/strategy/Classes/warrior/`, `druid/`
- 대상 클라이언트: MoP 5.4.8 build 18414

## 다음 작업
1. Legends Warrior/Druid class template와 AiFactory 연결 방식 정밀 대조
2. MoP 5.4.8 Monk spell/spec 자료 검증
3. Legends 경로 기준 Monk AI skeleton + Strategy/Trigger/Action overlay 작성
4. 정적 컴파일 오류 후보 검토 후 빌드 가능한 환경에서 PLAYERBOTS=1 검증
