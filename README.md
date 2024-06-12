# 물체 감지 CCTV

<img width = "40%" src="https://private-user-images.githubusercontent.com/159260193/339040495-05c3fa03-3a79-4037-918a-ac47c4e1b544.jpg?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MTgyMDc0NTIsIm5iZiI6MTcxODIwNzE1MiwicGF0aCI6Ii8xNTkyNjAxOTMvMzM5MDQwNDk1LTA1YzNmYTAzLTNhNzktNDAzNy05MThhLWFjNDdjNGUxYjU0NC5qcGc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjQwNjEyJTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI0MDYxMlQxNTQ1NTJaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT05ZGMxYmE0NGQzZDZiMzUzOGM3ZGZlYWFiZGE1NDQ5YmFiYjEzYjkxYWJhYjRhYWMyYjZlY2U5MWJhNTVkZjM1JlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZhY3Rvcl9pZD0wJmtleV9pZD0wJnJlcG9faWQ9MCJ9.wNF8WbD7uvL4cy2c9dDzEZjUrorcP87Ir4VXWKiq3yA"> 

## 1. 주요 기능
### 1.1 실시간 감시 CCTV
 * 640*480 30fps
 * OV7670 카메라 모듈을 활용한 실시간 감시
 * Basys 3의 VGA 커넥터를 통해 모니터로 출력
### 1.2 근접한 물체 감지
 * 초음파 센서를 이용해 전방의 물체를 감지
 * 자동회전 모드시 물체를 감지하면 제자리 고정
 * 설정한 특정 거리 이내의 물체를 감지하여 경고 발생
### 1.3 블루투스 송수신
 * 물체 감지시 블루투스로 경고 메시지 전송
 * 블루투스로 자동회전/수동회전 모드 전환
### 1.4 자동 회전
 * 스텝모터를 활용해 전방위 자동 회전 검색
### 1.5 수동 회전
 * 수동모드 전환시 조이스틱을 통해 회전 조작 가능
## 2. 동작 영상
 * 수동 모드 https://youtu.be/PVMzZIoBT7M 
 * 자동 모드 https://youtu.be/Yk3DqUKsr6w