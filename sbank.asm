;
; Sequence Offset Table
;

	dc.b	$04,$00		; Offset to sequence #0: 4
	dc.b	$05,$00		; Offset to sequence #1: 5
;
; Sequence Descriptors
;

; Sequence 0 "BLANK"
	dc.b	$00			; channel count = 0
; Sequence 1 "INSTRUMENT TEST"
	dc.b	$01			; channel count = 1
	dc.b	$08,$00		; offset to channel 0 = 8
;
; Sequence Channel Data
;

; Sequence 1, Channel 0:
	dc.b	$C0,$61,$00,$B0,$C1,$E0,$30,$C0
	dc.b	$61,$01,$C1,$E0,$30,$C0,$61,$02
	dc.b	$C1,$E0,$30,$C0,$61,$03,$C1,$E0
	dc.b	$30,$C0,$61,$04,$C1,$E0,$30,$C0
	dc.b	$61,$05,$C1,$E0,$30,$C0,$61,$06
	dc.b	$C1,$E0,$30,$C0,$61,$07,$C1,$E0
	dc.b	$30,$C0,$61,$08,$C1,$E0,$30,$C0
	dc.b	$61,$09,$C1,$E0,$30,$C0,$61,$0A
	dc.b	$C1,$E0,$30,$C0,$61,$0B,$C1,$E0
	dc.b	$30,$C0,$61,$0C,$C1,$E0,$30,$C0
	dc.b	$61,$0D,$C1,$E0,$30,$C0,$61,$0E
	dc.b	$C1,$E0,$30,$C0,$61,$0F,$C1,$E0
	dc.b	$30,$C0,$61,$10,$C1,$E0,$30,$C0
	dc.b	$61,$11,$C1,$E0,$30,$C0,$61,$12
	dc.b	$C1,$E0,$30,$C0,$61,$13,$C1,$E0
	dc.b	$30,$C0,$61,$14,$C1,$E0,$30,$C0
	dc.b	$61,$15,$C1,$E0,$30,$C0,$61,$16
	dc.b	$C1,$E0,$30,$C0,$61,$17,$C1,$E0
	dc.b	$30,$C0,$61,$18,$C1,$E0,$30,$C0
	dc.b	$61,$19,$C1,$E0,$30,$C0,$61,$1A
	dc.b	$C1,$E0,$30,$C0,$61,$1B,$C1,$E0
	dc.b	$30,$C0,$61,$1C,$C1,$E0,$30,$C0
	dc.b	$61,$1D,$C1,$E0,$30,$C0,$61,$1E
	dc.b	$C1,$E0,$30,$C0,$61,$1F,$C1,$E0
	dc.b	$30,$C0,$61,$20,$C1,$E0,$30,$C0
	dc.b	$61,$21,$C1,$E0,$30,$C0,$61,$22
	dc.b	$C1,$E0,$30,$C0,$61,$23,$C1,$E0
	dc.b	$30,$C0,$61,$24,$C1,$E0,$30,$C0
	dc.b	$61,$25,$C1,$E0,$30,$C0,$61,$26
	dc.b	$C1,$E0,$30,$C0,$61,$27,$C1,$E0
	dc.b	$30,$C0,$61,$28,$C1,$E0,$30,$C0
	dc.b	$61,$29,$C1,$E0,$30,$C0,$61,$2A
	dc.b	$C1,$E0,$30,$C0,$61,$2B,$C1,$E0
	dc.b	$30,$C0,$61,$2C,$C1,$E0,$30,$C0
	dc.b	$61,$2D,$C1,$E0,$30,$C0,$61,$2E
	dc.b	$C1,$E0,$30,$C0,$61,$2F,$C1,$E0
	dc.b	$30,$C0,$61,$30,$C1,$E0,$30,$C0
	dc.b	$61,$31,$C1,$E0,$30,$C0,$61,$32
	dc.b	$C1,$E0,$30,$C0,$61,$33,$C1,$E0
	dc.b	$30,$C0,$61,$34,$C1,$E0,$30,$C0
	dc.b	$61,$35,$C1,$E0,$30,$C0,$61,$36
	dc.b	$C1,$E1,$30,$C0,$61,$37,$AF,$C1
	dc.b	$DF,$30,$C0,$61,$38,$B0,$C1,$E0
	dc.b	$30,$C0,$61,$39,$C1,$E0,$30,$C0
	dc.b	$61,$3A,$C1,$E0,$30,$C0,$61,$3B
	dc.b	$C1,$E0,$30,$C0,$61,$3C,$C1,$E0
	dc.b	$30,$C0,$61,$3D,$C1,$E0,$30,$C0
	dc.b	$61,$3E,$C1,$E0,$30,$C0,$61,$3F
	dc.b	$C1,$E0,$30,$C0,$61,$40,$C1,$E0
	dc.b	$30,$C0,$61,$41,$C1,$E0,$30,$C0
	dc.b	$61,$42,$C1,$E0,$30,$C0,$61,$43
	dc.b	$C1,$E0,$30,$C0,$61,$44,$C1,$E0
	dc.b	$30,$C0,$61,$45,$C1,$E0,$30,$C0
	dc.b	$61,$46,$C1,$E0,$30,$C0,$61,$47
	dc.b	$C1,$E0,$30,$C0,$61,$48,$C1,$E0
	dc.b	$30,$C0,$61,$49,$C1,$E0,$30,$C0
	dc.b	$61,$4A,$C1,$E0,$30,$C0,$61,$4B
	dc.b	$C1,$E0,$30,$C0,$61,$4C,$C1,$E0
	dc.b	$30,$C0,$61,$4D,$C1,$E0,$30,$C0
	dc.b	$61,$4E,$C1,$E0,$30,$C0,$61,$4F
	dc.b	$C1,$E0,$30,$C0,$61,$50,$C1,$E0
	dc.b	$30,$C0,$61,$51,$C1,$E0,$30,$C0
	dc.b	$61,$52,$C1,$E0,$30,$C0,$61,$53
	dc.b	$C1,$E0,$30,$C0,$61,$54,$C1,$E0
	dc.b	$30,$C0,$61,$55,$C1,$E0,$30,$C0
	dc.b	$61,$56,$C1,$E0,$30,$C0,$61,$57
	dc.b	$C1,$E0,$30,$C0,$61,$58,$C1,$E0
	dc.b	$30,$C0,$61,$59,$C1,$E0,$30,$C0
	dc.b	$61,$5A,$C1,$E0,$30,$C0,$61,$5B
	dc.b	$C1,$E0,$30,$C0,$61,$5C,$C1,$E0
	dc.b	$30,$C0,$61,$5D,$C1,$E0,$30,$C0
	dc.b	$61,$5E,$C1,$E0,$30,$C0,$61,$5F
	dc.b	$C1,$E0,$30,$C0,$61,$60,$C1,$E0
	dc.b	$30,$C0,$61,$61,$C1,$E0,$30,$C0
	dc.b	$61,$62,$C1,$E0,$30,$C0,$61,$63
	dc.b	$C1,$E0,$30,$C0,$61,$64,$C1,$E0
	dc.b	$30,$C0,$61,$65,$C1,$E0,$30,$C0
	dc.b	$61,$66,$C1,$E0,$30,$C0,$61,$67
	dc.b	$C1,$E0,$30,$C0,$61,$68,$F0,$30
	dc.b	$60
