.CODE
	extern OnBreakPointCallback:PROC

OnBreakPointCallbackA PROC
	lea rsp,[rsp-108h]

	mov qword ptr [rsp+020h],rax
	mov qword ptr [rsp+028h],rcx
	mov qword ptr [rsp+030h],rdx
	mov qword ptr [rsp+038h],rbx
	mov qword ptr [rsp+040h],0
	mov qword ptr [rsp+048h],rbp
	mov qword ptr [rsp+050h],rsi
	mov qword ptr [rsp+058h],rdi
	mov qword ptr [rsp+060h],r8
	mov qword ptr [rsp+068h],r9
	mov qword ptr [rsp+070h],r10
	mov qword ptr [rsp+078h],r11
	mov qword ptr [rsp+080h],r12
	mov qword ptr [rsp+088h],r13
	mov qword ptr [rsp+090h],r14
	mov qword ptr [rsp+098h],r15

	lea rcx,[rsp+020h]
	lea rdx,[rsp+108h]
	call OnBreakPointCallback
	
	mov qword ptr r15,[rsp+098h]
	mov qword ptr r14,[rsp+090h]
	mov qword ptr r13,[rsp+088h]
	mov qword ptr r12,[rsp+080h]
	mov qword ptr r11,[rsp+078h]
	mov qword ptr r10,[rsp+070h]
	mov qword ptr r9,[rsp+068h]
	mov qword ptr r8,[rsp+060h]
	mov qword ptr rdi,[rsp+058h]
	mov qword ptr rsi,[rsp+050h]
	mov qword ptr rbp,[rsp+048h]
	;mov qword ptr rsp,[rsp+040h]
	mov qword ptr rbx,[rsp+038h]
	mov qword ptr rdx,[rsp+030h]
	mov qword ptr rcx,[rsp+028h]
	mov qword ptr rax,[rsp+020h]

	add rsp,[rsp+108h]
	iret
OnBreakPointCallbackA ENDP

END