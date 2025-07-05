
.\build\esp-idf\main\ulp_core_main\ulp_core_main.elf:     file format elf32-littleriscv


Disassembly of section .vector.text:

50000000 <_vector_table>:
50000000:	5960006f          	j	50000596 <_panic_handler>
50000004:	5920006f          	j	50000596 <_panic_handler>
50000008:	58e0006f          	j	50000596 <_panic_handler>
5000000c:	58a0006f          	j	50000596 <_panic_handler>
50000010:	5860006f          	j	50000596 <_panic_handler>
50000014:	5820006f          	j	50000596 <_panic_handler>
50000018:	57e0006f          	j	50000596 <_panic_handler>
5000001c:	57a0006f          	j	50000596 <_panic_handler>
50000020:	5760006f          	j	50000596 <_panic_handler>
50000024:	5720006f          	j	50000596 <_panic_handler>
50000028:	56e0006f          	j	50000596 <_panic_handler>
5000002c:	56a0006f          	j	50000596 <_panic_handler>
50000030:	5660006f          	j	50000596 <_panic_handler>
50000034:	5620006f          	j	50000596 <_panic_handler>
50000038:	55e0006f          	j	50000596 <_panic_handler>
5000003c:	55a0006f          	j	50000596 <_panic_handler>
50000040:	5560006f          	j	50000596 <_panic_handler>
50000044:	5520006f          	j	50000596 <_panic_handler>
50000048:	54e0006f          	j	50000596 <_panic_handler>
5000004c:	54a0006f          	j	50000596 <_panic_handler>
50000050:	5460006f          	j	50000596 <_panic_handler>
50000054:	5420006f          	j	50000596 <_panic_handler>
50000058:	53e0006f          	j	50000596 <_panic_handler>
5000005c:	53a0006f          	j	50000596 <_panic_handler>
50000060:	5360006f          	j	50000596 <_panic_handler>
50000064:	5320006f          	j	50000596 <_panic_handler>
50000068:	52e0006f          	j	50000596 <_panic_handler>
5000006c:	52a0006f          	j	50000596 <_panic_handler>
50000070:	5260006f          	j	50000596 <_panic_handler>
50000074:	5220006f          	j	50000596 <_panic_handler>
50000078:	58e0006f          	j	50000606 <_interrupt_handler>
5000007c:	51a0006f          	j	50000596 <_panic_handler>

Disassembly of section .text:

50000080 <reset_vector>:
50000080:	00000297          	auipc	t0,0x0
50000084:	f8028293          	addi	t0,t0,-128 # 50000000 <_vector_table>
50000088:	30529073          	csrw	mtvec,t0
5000008c:	a009                	j	5000008e <__start>

5000008e <__start>:
5000008e:	00001117          	auipc	sp,0x1
50000092:	f6210113          	addi	sp,sp,-158 # 50000ff0 <s_shared_mem>
50000096:	25ed                	jal	50000780 <lp_core_startup>

50000098 <loop>:
50000098:	a001                	j	50000098 <loop>

5000009a <sleep_store>:
5000009a:	7139                	addi	sp,sp,-64
5000009c:	c006                	sw	ra,0(sp)
5000009e:	c20e                	sw	gp,4(sp)
500000a0:	c412                	sw	tp,8(sp)
500000a2:	c822                	sw	s0,16(sp)
500000a4:	ca26                	sw	s1,20(sp)
500000a6:	cc4a                	sw	s2,24(sp)
500000a8:	ce4e                	sw	s3,28(sp)
500000aa:	d052                	sw	s4,32(sp)
500000ac:	d256                	sw	s5,36(sp)
500000ae:	036128a3          	sw	s6,49(sp)
500000b2:	d65e                	sw	s7,44(sp)
500000b4:	d862                	sw	s8,48(sp)
500000b6:	da66                	sw	s9,52(sp)
500000b8:	dc6a                	sw	s10,56(sp)
500000ba:	de6e                	sw	s11,60(sp)
500000bc:	00001517          	auipc	a0,0x1
500000c0:	e7050513          	addi	a0,a0,-400 # 50000f2c <mrbc_sp_bottom>
500000c4:	00252023          	sw	sp,0(a0)
500000c8:	39c0006f          	j	50000464 <sleep_store2>

500000cc <sleep_restore>:
500000cc:	00001517          	auipc	a0,0x1
500000d0:	e6050513          	addi	a0,a0,-416 # 50000f2c <mrbc_sp_bottom>
500000d4:	00052103          	lw	sp,0(a0)
500000d8:	4082                	lw	ra,0(sp)
500000da:	4192                	lw	gp,4(sp)
500000dc:	4222                	lw	tp,8(sp)
500000de:	4442                	lw	s0,16(sp)
500000e0:	44d2                	lw	s1,20(sp)
500000e2:	4962                	lw	s2,24(sp)
500000e4:	49f2                	lw	s3,28(sp)
500000e6:	5a02                	lw	s4,32(sp)
500000e8:	5a92                	lw	s5,36(sp)
500000ea:	03112b03          	lw	s6,49(sp)
500000ee:	5bb2                	lw	s7,44(sp)
500000f0:	5c42                	lw	s8,48(sp)
500000f2:	5cd2                	lw	s9,52(sp)
500000f4:	5d62                	lw	s10,56(sp)
500000f6:	5df2                	lw	s11,60(sp)
500000f8:	6121                	addi	sp,sp,64
500000fa:	00008067          	ret

500000fe <__udivdi3>:
500000fe:	88aa                	mv	a7,a0
50000100:	832e                	mv	t1,a1
50000102:	8732                	mv	a4,a2
50000104:	882a                	mv	a6,a0
50000106:	87ae                	mv	a5,a1
50000108:	20069663          	bnez	a3,50000314 <__udivdi3+0x216>
5000010c:	500015b7          	lui	a1,0x50001
50000110:	d1458593          	addi	a1,a1,-748 # 50000d14 <__clz_tab>
50000114:	0cc37163          	bgeu	t1,a2,500001d6 <__udivdi3+0xd8>
50000118:	66c1                	lui	a3,0x10
5000011a:	0ad67763          	bgeu	a2,a3,500001c8 <__udivdi3+0xca>
5000011e:	10063693          	sltiu	a3,a2,256
50000122:	0016b693          	seqz	a3,a3
50000126:	068e                	slli	a3,a3,0x3
50000128:	00d65533          	srl	a0,a2,a3
5000012c:	95aa                	add	a1,a1,a0
5000012e:	0005c583          	lbu	a1,0(a1)
50000132:	02000513          	li	a0,32
50000136:	96ae                	add	a3,a3,a1
50000138:	40d505b3          	sub	a1,a0,a3
5000013c:	00d50b63          	beq	a0,a3,50000152 <__udivdi3+0x54>
50000140:	00b317b3          	sll	a5,t1,a1
50000144:	00d8d6b3          	srl	a3,a7,a3
50000148:	00b61733          	sll	a4,a2,a1
5000014c:	8fd5                	or	a5,a5,a3
5000014e:	00b89833          	sll	a6,a7,a1
50000152:	01075593          	srli	a1,a4,0x10
50000156:	02b7d333          	divu	t1,a5,a1
5000015a:	01071613          	slli	a2,a4,0x10
5000015e:	8241                	srli	a2,a2,0x10
50000160:	02b7f7b3          	remu	a5,a5,a1
50000164:	851a                	mv	a0,t1
50000166:	026608b3          	mul	a7,a2,t1
5000016a:	01079693          	slli	a3,a5,0x10
5000016e:	01085793          	srli	a5,a6,0x10
50000172:	8fd5                	or	a5,a5,a3
50000174:	0117fc63          	bgeu	a5,a7,5000018c <__udivdi3+0x8e>
50000178:	97ba                	add	a5,a5,a4
5000017a:	fff30513          	addi	a0,t1,-1
5000017e:	00e7e763          	bltu	a5,a4,5000018c <__udivdi3+0x8e>
50000182:	0117f563          	bgeu	a5,a7,5000018c <__udivdi3+0x8e>
50000186:	ffe30513          	addi	a0,t1,-2
5000018a:	97ba                	add	a5,a5,a4
5000018c:	411787b3          	sub	a5,a5,a7
50000190:	02b7d8b3          	divu	a7,a5,a1
50000194:	0842                	slli	a6,a6,0x10
50000196:	01085813          	srli	a6,a6,0x10
5000019a:	02b7f7b3          	remu	a5,a5,a1
5000019e:	031606b3          	mul	a3,a2,a7
500001a2:	07c2                	slli	a5,a5,0x10
500001a4:	00f86833          	or	a6,a6,a5
500001a8:	87c6                	mv	a5,a7
500001aa:	00d87b63          	bgeu	a6,a3,500001c0 <__udivdi3+0xc2>
500001ae:	983a                	add	a6,a6,a4
500001b0:	fff88793          	addi	a5,a7,-1
500001b4:	00e86663          	bltu	a6,a4,500001c0 <__udivdi3+0xc2>
500001b8:	00d87463          	bgeu	a6,a3,500001c0 <__udivdi3+0xc2>
500001bc:	ffe88793          	addi	a5,a7,-2
500001c0:	0542                	slli	a0,a0,0x10
500001c2:	8d5d                	or	a0,a0,a5
500001c4:	4581                	li	a1,0
500001c6:	8082                	ret
500001c8:	01000537          	lui	a0,0x1000
500001cc:	46e1                	li	a3,24
500001ce:	f4a67de3          	bgeu	a2,a0,50000128 <__udivdi3+0x2a>
500001d2:	46c1                	li	a3,16
500001d4:	bf91                	j	50000128 <__udivdi3+0x2a>
500001d6:	4681                	li	a3,0
500001d8:	ca09                	beqz	a2,500001ea <__udivdi3+0xec>
500001da:	67c1                	lui	a5,0x10
500001dc:	08f67f63          	bgeu	a2,a5,5000027a <__udivdi3+0x17c>
500001e0:	10063693          	sltiu	a3,a2,256
500001e4:	0016b693          	seqz	a3,a3
500001e8:	068e                	slli	a3,a3,0x3
500001ea:	00d657b3          	srl	a5,a2,a3
500001ee:	95be                	add	a1,a1,a5
500001f0:	0005c783          	lbu	a5,0(a1)
500001f4:	97b6                	add	a5,a5,a3
500001f6:	02000693          	li	a3,32
500001fa:	40f685b3          	sub	a1,a3,a5
500001fe:	08f69563          	bne	a3,a5,50000288 <__udivdi3+0x18a>
50000202:	40c307b3          	sub	a5,t1,a2
50000206:	4585                	li	a1,1
50000208:	01075893          	srli	a7,a4,0x10
5000020c:	0317de33          	divu	t3,a5,a7
50000210:	01071613          	slli	a2,a4,0x10
50000214:	8241                	srli	a2,a2,0x10
50000216:	01085693          	srli	a3,a6,0x10
5000021a:	0317f7b3          	remu	a5,a5,a7
5000021e:	8572                	mv	a0,t3
50000220:	03c60333          	mul	t1,a2,t3
50000224:	07c2                	slli	a5,a5,0x10
50000226:	8fd5                	or	a5,a5,a3
50000228:	0067fc63          	bgeu	a5,t1,50000240 <__udivdi3+0x142>
5000022c:	97ba                	add	a5,a5,a4
5000022e:	fffe0513          	addi	a0,t3,-1
50000232:	00e7e763          	bltu	a5,a4,50000240 <__udivdi3+0x142>
50000236:	0067f563          	bgeu	a5,t1,50000240 <__udivdi3+0x142>
5000023a:	ffee0513          	addi	a0,t3,-2
5000023e:	97ba                	add	a5,a5,a4
50000240:	406787b3          	sub	a5,a5,t1
50000244:	0317d333          	divu	t1,a5,a7
50000248:	0842                	slli	a6,a6,0x10
5000024a:	01085813          	srli	a6,a6,0x10
5000024e:	0317f7b3          	remu	a5,a5,a7
50000252:	026606b3          	mul	a3,a2,t1
50000256:	07c2                	slli	a5,a5,0x10
50000258:	00f86833          	or	a6,a6,a5
5000025c:	879a                	mv	a5,t1
5000025e:	00d87b63          	bgeu	a6,a3,50000274 <__udivdi3+0x176>
50000262:	983a                	add	a6,a6,a4
50000264:	fff30793          	addi	a5,t1,-1
50000268:	00e86663          	bltu	a6,a4,50000274 <__udivdi3+0x176>
5000026c:	00d87463          	bgeu	a6,a3,50000274 <__udivdi3+0x176>
50000270:	ffe30793          	addi	a5,t1,-2
50000274:	0542                	slli	a0,a0,0x10
50000276:	8d5d                	or	a0,a0,a5
50000278:	8082                	ret
5000027a:	010007b7          	lui	a5,0x1000
5000027e:	46e1                	li	a3,24
50000280:	f6f675e3          	bgeu	a2,a5,500001ea <__udivdi3+0xec>
50000284:	46c1                	li	a3,16
50000286:	b795                	j	500001ea <__udivdi3+0xec>
50000288:	00b61733          	sll	a4,a2,a1
5000028c:	00f356b3          	srl	a3,t1,a5
50000290:	01075513          	srli	a0,a4,0x10
50000294:	00b31333          	sll	t1,t1,a1
50000298:	00f8d7b3          	srl	a5,a7,a5
5000029c:	0067e7b3          	or	a5,a5,t1
500002a0:	02a6d333          	divu	t1,a3,a0
500002a4:	01071613          	slli	a2,a4,0x10
500002a8:	8241                	srli	a2,a2,0x10
500002aa:	00b89833          	sll	a6,a7,a1
500002ae:	02a6f6b3          	remu	a3,a3,a0
500002b2:	026608b3          	mul	a7,a2,t1
500002b6:	01069593          	slli	a1,a3,0x10
500002ba:	0107d693          	srli	a3,a5,0x10
500002be:	8ecd                	or	a3,a3,a1
500002c0:	859a                	mv	a1,t1
500002c2:	0116fc63          	bgeu	a3,a7,500002da <__udivdi3+0x1dc>
500002c6:	96ba                	add	a3,a3,a4
500002c8:	fff30593          	addi	a1,t1,-1
500002cc:	00e6e763          	bltu	a3,a4,500002da <__udivdi3+0x1dc>
500002d0:	0116f563          	bgeu	a3,a7,500002da <__udivdi3+0x1dc>
500002d4:	ffe30593          	addi	a1,t1,-2
500002d8:	96ba                	add	a3,a3,a4
500002da:	411686b3          	sub	a3,a3,a7
500002de:	02a6d8b3          	divu	a7,a3,a0
500002e2:	07c2                	slli	a5,a5,0x10
500002e4:	83c1                	srli	a5,a5,0x10
500002e6:	02a6f6b3          	remu	a3,a3,a0
500002ea:	03160633          	mul	a2,a2,a7
500002ee:	06c2                	slli	a3,a3,0x10
500002f0:	8fd5                	or	a5,a5,a3
500002f2:	86c6                	mv	a3,a7
500002f4:	00c7fc63          	bgeu	a5,a2,5000030c <__udivdi3+0x20e>
500002f8:	97ba                	add	a5,a5,a4
500002fa:	fff88693          	addi	a3,a7,-1
500002fe:	00e7e763          	bltu	a5,a4,5000030c <__udivdi3+0x20e>
50000302:	00c7f563          	bgeu	a5,a2,5000030c <__udivdi3+0x20e>
50000306:	ffe88693          	addi	a3,a7,-2
5000030a:	97ba                	add	a5,a5,a4
5000030c:	05c2                	slli	a1,a1,0x10
5000030e:	8f91                	sub	a5,a5,a2
50000310:	8dd5                	or	a1,a1,a3
50000312:	bddd                	j	50000208 <__udivdi3+0x10a>
50000314:	12d5ef63          	bltu	a1,a3,50000452 <__udivdi3+0x354>
50000318:	67c1                	lui	a5,0x10
5000031a:	02f6ff63          	bgeu	a3,a5,50000358 <__udivdi3+0x25a>
5000031e:	1006b793          	sltiu	a5,a3,256
50000322:	0017b793          	seqz	a5,a5
50000326:	078e                	slli	a5,a5,0x3
50000328:	50001737          	lui	a4,0x50001
5000032c:	00f6d5b3          	srl	a1,a3,a5
50000330:	d1470713          	addi	a4,a4,-748 # 50000d14 <__clz_tab>
50000334:	972e                	add	a4,a4,a1
50000336:	00074703          	lbu	a4,0(a4)
5000033a:	973e                	add	a4,a4,a5
5000033c:	02000793          	li	a5,32
50000340:	40e785b3          	sub	a1,a5,a4
50000344:	02e79163          	bne	a5,a4,50000366 <__udivdi3+0x268>
50000348:	4505                	li	a0,1
5000034a:	e666eee3          	bltu	a3,t1,500001c6 <__udivdi3+0xc8>
5000034e:	00c8b533          	sltu	a0,a7,a2
50000352:	00153513          	seqz	a0,a0
50000356:	8082                	ret
50000358:	01000737          	lui	a4,0x1000
5000035c:	47e1                	li	a5,24
5000035e:	fce6f5e3          	bgeu	a3,a4,50000328 <__udivdi3+0x22a>
50000362:	47c1                	li	a5,16
50000364:	b7d1                	j	50000328 <__udivdi3+0x22a>
50000366:	00e65533          	srl	a0,a2,a4
5000036a:	00b696b3          	sll	a3,a3,a1
5000036e:	00e357b3          	srl	a5,t1,a4
50000372:	8d55                	or	a0,a0,a3
50000374:	00b31333          	sll	t1,t1,a1
50000378:	00e8d733          	srl	a4,a7,a4
5000037c:	00676733          	or	a4,a4,t1
50000380:	01055313          	srli	t1,a0,0x10
50000384:	0267deb3          	divu	t4,a5,t1
50000388:	01051813          	slli	a6,a0,0x10
5000038c:	01085813          	srli	a6,a6,0x10
50000390:	01075693          	srli	a3,a4,0x10
50000394:	00b61633          	sll	a2,a2,a1
50000398:	0267f7b3          	remu	a5,a5,t1
5000039c:	03d80e33          	mul	t3,a6,t4
500003a0:	07c2                	slli	a5,a5,0x10
500003a2:	8edd                	or	a3,a3,a5
500003a4:	87f6                	mv	a5,t4
500003a6:	01c6fc63          	bgeu	a3,t3,500003be <__udivdi3+0x2c0>
500003aa:	96aa                	add	a3,a3,a0
500003ac:	fffe8793          	addi	a5,t4,-1
500003b0:	00a6e763          	bltu	a3,a0,500003be <__udivdi3+0x2c0>
500003b4:	01c6f563          	bgeu	a3,t3,500003be <__udivdi3+0x2c0>
500003b8:	ffee8793          	addi	a5,t4,-2
500003bc:	96aa                	add	a3,a3,a0
500003be:	41c686b3          	sub	a3,a3,t3
500003c2:	0266de33          	divu	t3,a3,t1
500003c6:	0742                	slli	a4,a4,0x10
500003c8:	8341                	srli	a4,a4,0x10
500003ca:	0266f6b3          	remu	a3,a3,t1
500003ce:	03c80833          	mul	a6,a6,t3
500003d2:	06c2                	slli	a3,a3,0x10
500003d4:	8f55                	or	a4,a4,a3
500003d6:	86f2                	mv	a3,t3
500003d8:	01077c63          	bgeu	a4,a6,500003f0 <__udivdi3+0x2f2>
500003dc:	972a                	add	a4,a4,a0
500003de:	fffe0693          	addi	a3,t3,-1
500003e2:	00a76763          	bltu	a4,a0,500003f0 <__udivdi3+0x2f2>
500003e6:	01077563          	bgeu	a4,a6,500003f0 <__udivdi3+0x2f2>
500003ea:	ffee0693          	addi	a3,t3,-2
500003ee:	972a                	add	a4,a4,a0
500003f0:	07c2                	slli	a5,a5,0x10
500003f2:	00d7e533          	or	a0,a5,a3
500003f6:	01061313          	slli	t1,a2,0x10
500003fa:	06c2                	slli	a3,a3,0x10
500003fc:	82c1                	srli	a3,a3,0x10
500003fe:	01035313          	srli	t1,t1,0x10
50000402:	8241                	srli	a2,a2,0x10
50000404:	41070733          	sub	a4,a4,a6
50000408:	01055813          	srli	a6,a0,0x10
5000040c:	02668e33          	mul	t3,a3,t1
50000410:	02680333          	mul	t1,a6,t1
50000414:	010e5793          	srli	a5,t3,0x10
50000418:	02c686b3          	mul	a3,a3,a2
5000041c:	969a                	add	a3,a3,t1
5000041e:	97b6                	add	a5,a5,a3
50000420:	02c80833          	mul	a6,a6,a2
50000424:	0067f463          	bgeu	a5,t1,5000042c <__udivdi3+0x32e>
50000428:	66c1                	lui	a3,0x10
5000042a:	9836                	add	a6,a6,a3
5000042c:	0107d693          	srli	a3,a5,0x10
50000430:	96c2                	add	a3,a3,a6
50000432:	00d76e63          	bltu	a4,a3,5000044e <__udivdi3+0x350>
50000436:	d8d717e3          	bne	a4,a3,500001c4 <__udivdi3+0xc6>
5000043a:	0e42                	slli	t3,t3,0x10
5000043c:	07c2                	slli	a5,a5,0x10
5000043e:	010e5e13          	srli	t3,t3,0x10
50000442:	00b898b3          	sll	a7,a7,a1
50000446:	97f2                	add	a5,a5,t3
50000448:	4581                	li	a1,0
5000044a:	d6f8fee3          	bgeu	a7,a5,500001c6 <__udivdi3+0xc8>
5000044e:	157d                	addi	a0,a0,-1 # ffffff <RvExcFrameSize+0xffff6b>
50000450:	bb95                	j	500001c4 <__udivdi3+0xc6>
50000452:	4581                	li	a1,0
50000454:	4501                	li	a0,0
50000456:	8082                	ret

50000458 <ulp_lp_core_lp_timer_intr_handler>:
50000458:	1141                	addi	sp,sp,-16
5000045a:	c606                	sw	ra,12(sp)
5000045c:	21cd                	jal	5000093e <ulp_lp_core_lp_timer_intr_clear>
5000045e:	40b2                	lw	ra,12(sp)
50000460:	0141                	addi	sp,sp,16
50000462:	a601                	j	50000762 <ulp_lp_core_lp_timer_disable>

50000464 <sleep_store2>:
50000464:	1141                	addi	sp,sp,-16
50000466:	c606                	sw	ra,12(sp)
50000468:	2969                	jal	50000902 <ulp_lp_core_halt>

5000046a <wakeup>:
5000046a:	1141                	addi	sp,sp,-16
5000046c:	c606                	sw	ra,12(sp)
5000046e:	2cd5                	jal	50000762 <ulp_lp_core_lp_timer_disable>
50000470:	21bd                	jal	500008de <ulp_lp_core_wakeup_main_processor>
50000472:	2941                	jal	50000902 <ulp_lp_core_halt>

50000474 <sleep>:
50000474:	1141                	addi	sp,sp,-16
50000476:	600b07b7          	lui	a5,0x600b0
5000047a:	c606                	sw	ra,12(sp)
5000047c:	00078793          	mv	a5,a5
50000480:	4741                	li	a4,16
50000482:	18e79023          	sh	a4,384(a5) # 600b0180 <PMU+0x180>
50000486:	3e800793          	li	a5,1000
5000048a:	02f50533          	mul	a0,a0,a5
5000048e:	41f55593          	srai	a1,a0,0x1f
50000492:	249d                	jal	500006f8 <ulp_lp_core_lp_timer_set_wakeup_time>
50000494:	40b2                	lw	ra,12(sp)
50000496:	0141                	addi	sp,sp,16
50000498:	b109                	j	5000009a <sleep_store>

5000049a <read_sensor>:
5000049a:	1101                	addi	sp,sp,-32
5000049c:	50001637          	lui	a2,0x50001
500004a0:	cc22                	sw	s0,24(sp)
500004a2:	577d                	li	a4,-1
500004a4:	842a                	mv	s0,a0
500004a6:	4689                	li	a3,2
500004a8:	e2c60613          	addi	a2,a2,-468 # 50000e2c <LOW_REPEAT_READ>
500004ac:	04400593          	li	a1,68
500004b0:	4505                	li	a0,1
500004b2:	ce06                	sw	ra,28(sp)
500004b4:	2721                	jal	50000bbc <lp_core_i2c_master_write_to_device>
500004b6:	4519                	li	a0,6
500004b8:	3f75                	jal	50000474 <sleep>
500004ba:	0030                	addi	a2,sp,8
500004bc:	577d                	li	a4,-1
500004be:	4699                	li	a3,6
500004c0:	04400593          	li	a1,68
500004c4:	4505                	li	a0,1
500004c6:	2b55                	jal	50000a7a <lp_core_i2c_master_read_from_device>
500004c8:	00c14783          	lbu	a5,12(sp)
500004cc:	00b14703          	lbu	a4,11(sp)
500004d0:	00815603          	lhu	a2,8(sp)
500004d4:	07a2                	slli	a5,a5,0x8
500004d6:	8f5d                	or	a4,a4,a5
500004d8:	0722                	slli	a4,a4,0x8
500004da:	83a1                	srli	a5,a5,0x8
500004dc:	8fd9                	or	a5,a5,a4
500004de:	00865713          	srli	a4,a2,0x8
500004e2:	0622                	slli	a2,a2,0x8
500004e4:	8f51                	or	a4,a4,a2
500004e6:	0742                	slli	a4,a4,0x10
500004e8:	0af00613          	li	a2,175
500004ec:	8341                	srli	a4,a4,0x10
500004ee:	02c70733          	mul	a4,a4,a2
500004f2:	6641                	lui	a2,0x10
500004f4:	167d                	addi	a2,a2,-1 # ffff <RvExcFrameSize+0xff6b>
500004f6:	500016b7          	lui	a3,0x50001
500004fa:	00241513          	slli	a0,s0,0x2
500004fe:	eb068693          	addi	a3,a3,-336 # 50000eb0 <tmp>
50000502:	96aa                	add	a3,a3,a0
50000504:	07c2                	slli	a5,a5,0x10
50000506:	83c1                	srli	a5,a5,0x10
50000508:	40f2                	lw	ra,28(sp)
5000050a:	02c74733          	div	a4,a4,a2
5000050e:	4462                	lw	s0,24(sp)
50000510:	fd370713          	addi	a4,a4,-45 # ffffd3 <RvExcFrameSize+0xffff3f>
50000514:	c298                	sw	a4,0(a3)
50000516:	06400693          	li	a3,100
5000051a:	02d787b3          	mul	a5,a5,a3
5000051e:	50001737          	lui	a4,0x50001
50000522:	e3870713          	addi	a4,a4,-456 # 50000e38 <rh>
50000526:	972a                	add	a4,a4,a0
50000528:	02c7c7b3          	div	a5,a5,a2
5000052c:	c31c                	sw	a5,0(a4)
5000052e:	6105                	addi	sp,sp,32
50000530:	8082                	ret

50000532 <app_main>:
50000532:	1141                	addi	sp,sp,-16
50000534:	c422                	sw	s0,8(sp)
50000536:	50001437          	lui	s0,0x50001
5000053a:	f2842503          	lw	a0,-216(s0) # 50000f28 <counter>
5000053e:	c606                	sw	ra,12(sp)
50000540:	3fa9                	jal	5000049a <read_sensor>
50000542:	f2842783          	lw	a5,-216(s0)
50000546:	4779                	li	a4,30
50000548:	0785                	addi	a5,a5,1
5000054a:	f2f42423          	sw	a5,-216(s0)
5000054e:	00e79363          	bne	a5,a4,50000554 <app_main+0x22>
50000552:	3f21                	jal	5000046a <wakeup>
50000554:	40b2                	lw	ra,12(sp)
50000556:	4422                	lw	s0,8(sp)
50000558:	4501                	li	a0,0
5000055a:	0141                	addi	sp,sp,16
5000055c:	8082                	ret

5000055e <main>:
5000055e:	1141                	addi	sp,sp,-16
50000560:	c422                	sw	s0,8(sp)
50000562:	50001437          	lui	s0,0x50001
50000566:	f2c42783          	lw	a5,-212(s0) # 50000f2c <mrbc_sp_bottom>
5000056a:	c606                	sw	ra,12(sp)
5000056c:	c391                	beqz	a5,50000570 <main+0x12>
5000056e:	3eb9                	jal	500000cc <sleep_restore>
50000570:	7119                	addi	sp,sp,-128
50000572:	37c1                	jal	50000532 <app_main>
50000574:	6109                	addi	sp,sp,128
50000576:	600b07b7          	lui	a5,0x600b0
5000057a:	000f4537          	lui	a0,0xf4
5000057e:	00078793          	mv	a5,a5
50000582:	4741                	li	a4,16
50000584:	24050513          	addi	a0,a0,576 # f4240 <RvExcFrameSize+0xf41ac>
50000588:	4581                	li	a1,0
5000058a:	18e79023          	sh	a4,384(a5) # 600b0180 <PMU+0x180>
5000058e:	f2042623          	sw	zero,-212(s0)
50000592:	229d                	jal	500006f8 <ulp_lp_core_lp_timer_set_wakeup_time>
50000594:	26bd                	jal	50000902 <ulp_lp_core_halt>

50000596 <_panic_handler>:
50000596:	7135                	addi	sp,sp,-160
50000598:	c206                	sw	ra,4(sp)
5000059a:	c812                	sw	tp,16(sp)
5000059c:	ca16                	sw	t0,20(sp)
5000059e:	cc1a                	sw	t1,24(sp)
500005a0:	ce1e                	sw	t2,28(sp)
500005a2:	d022                	sw	s0,32(sp)
500005a4:	d226                	sw	s1,36(sp)
500005a6:	d42a                	sw	a0,40(sp)
500005a8:	d62e                	sw	a1,44(sp)
500005aa:	d832                	sw	a2,48(sp)
500005ac:	da36                	sw	a3,52(sp)
500005ae:	dc3a                	sw	a4,56(sp)
500005b0:	de3e                	sw	a5,60(sp)
500005b2:	c0c2                	sw	a6,64(sp)
500005b4:	c2c6                	sw	a7,68(sp)
500005b6:	c4ca                	sw	s2,72(sp)
500005b8:	c6ce                	sw	s3,76(sp)
500005ba:	c8d2                	sw	s4,80(sp)
500005bc:	cad6                	sw	s5,84(sp)
500005be:	ccda                	sw	s6,88(sp)
500005c0:	cede                	sw	s7,92(sp)
500005c2:	d0e2                	sw	s8,96(sp)
500005c4:	d2e6                	sw	s9,100(sp)
500005c6:	d4ea                	sw	s10,104(sp)
500005c8:	d6ee                	sw	s11,108(sp)
500005ca:	d8f2                	sw	t3,112(sp)
500005cc:	daf6                	sw	t4,116(sp)
500005ce:	dcfa                	sw	t5,120(sp)
500005d0:	defe                	sw	t6,124(sp)
500005d2:	341022f3          	csrr	t0,mepc
500005d6:	c016                	sw	t0,0(sp)
500005d8:	0a010293          	addi	t0,sp,160
500005dc:	c416                	sw	t0,8(sp)
500005de:	300022f3          	csrr	t0,mstatus
500005e2:	c116                	sw	t0,128(sp)
500005e4:	342022f3          	csrr	t0,mcause
500005e8:	c516                	sw	t0,136(sp)
500005ea:	305022f3          	csrr	t0,mtvec
500005ee:	c316                	sw	t0,132(sp)
500005f0:	f14022f3          	csrr	t0,mhartid
500005f4:	c916                	sw	t0,144(sp)
500005f6:	343022f3          	csrr	t0,mtval
500005fa:	c716                	sw	t0,140(sp)
500005fc:	342025f3          	csrr	a1,mcause
50000600:	850a                	mv	a0,sp
50000602:	2e99                	jal	50000958 <ulp_lp_core_panic_handler>

50000604 <_end>:
50000604:	a001                	j	50000604 <_end>

50000606 <_interrupt_handler>:
50000606:	7119                	addi	sp,sp,-128
50000608:	c206                	sw	ra,4(sp)
5000060a:	c812                	sw	tp,16(sp)
5000060c:	ca16                	sw	t0,20(sp)
5000060e:	cc1a                	sw	t1,24(sp)
50000610:	ce1e                	sw	t2,28(sp)
50000612:	d022                	sw	s0,32(sp)
50000614:	d226                	sw	s1,36(sp)
50000616:	d42a                	sw	a0,40(sp)
50000618:	d62e                	sw	a1,44(sp)
5000061a:	d832                	sw	a2,48(sp)
5000061c:	da36                	sw	a3,52(sp)
5000061e:	dc3a                	sw	a4,56(sp)
50000620:	de3e                	sw	a5,60(sp)
50000622:	c0c2                	sw	a6,64(sp)
50000624:	c2c6                	sw	a7,68(sp)
50000626:	c4ca                	sw	s2,72(sp)
50000628:	c6ce                	sw	s3,76(sp)
5000062a:	c8d2                	sw	s4,80(sp)
5000062c:	cad6                	sw	s5,84(sp)
5000062e:	ccda                	sw	s6,88(sp)
50000630:	cede                	sw	s7,92(sp)
50000632:	d0e2                	sw	s8,96(sp)
50000634:	d2e6                	sw	s9,100(sp)
50000636:	d4ea                	sw	s10,104(sp)
50000638:	d6ee                	sw	s11,108(sp)
5000063a:	d8f2                	sw	t3,112(sp)
5000063c:	daf6                	sw	t4,116(sp)
5000063e:	dcfa                	sw	t5,120(sp)
50000640:	defe                	sw	t6,124(sp)
50000642:	341022f3          	csrr	t0,mepc
50000646:	c016                	sw	t0,0(sp)
50000648:	2e19                	jal	5000095e <ulp_lp_core_intr_handler>
5000064a:	4282                	lw	t0,0(sp)
5000064c:	34129073          	csrw	mepc,t0
50000650:	4092                	lw	ra,4(sp)
50000652:	4242                	lw	tp,16(sp)
50000654:	42d2                	lw	t0,20(sp)
50000656:	4362                	lw	t1,24(sp)
50000658:	43f2                	lw	t2,28(sp)
5000065a:	5402                	lw	s0,32(sp)
5000065c:	5492                	lw	s1,36(sp)
5000065e:	5522                	lw	a0,40(sp)
50000660:	55b2                	lw	a1,44(sp)
50000662:	5642                	lw	a2,48(sp)
50000664:	56d2                	lw	a3,52(sp)
50000666:	5762                	lw	a4,56(sp)
50000668:	57f2                	lw	a5,60(sp)
5000066a:	4806                	lw	a6,64(sp)
5000066c:	4896                	lw	a7,68(sp)
5000066e:	4926                	lw	s2,72(sp)
50000670:	49b6                	lw	s3,76(sp)
50000672:	4a46                	lw	s4,80(sp)
50000674:	4ad6                	lw	s5,84(sp)
50000676:	4b66                	lw	s6,88(sp)
50000678:	4bf6                	lw	s7,92(sp)
5000067a:	5c06                	lw	s8,96(sp)
5000067c:	5c96                	lw	s9,100(sp)
5000067e:	5d26                	lw	s10,104(sp)
50000680:	5db6                	lw	s11,108(sp)
50000682:	5e46                	lw	t3,112(sp)
50000684:	5ed6                	lw	t4,116(sp)
50000686:	5f66                	lw	t5,120(sp)
50000688:	5ff6                	lw	t6,124(sp)
5000068a:	6109                	addi	sp,sp,128
5000068c:	30200073          	mret

50000690 <ulp_lp_core_memory_shared_cfg_get>:
50000690:	50001537          	lui	a0,0x50001
50000694:	ff050513          	addi	a0,a0,-16 # 50000ff0 <s_shared_mem>
50000698:	8082                	ret

5000069a <lp_timer_hal_set_alarm_target>:
5000069a:	600b17b7          	lui	a5,0x600b1
5000069e:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
500006a2:	43f8                	lw	a4,68(a5)
500006a4:	800006b7          	lui	a3,0x80000
500006a8:	1141                	addi	sp,sp,-16
500006aa:	8f55                	or	a4,a4,a3
500006ac:	c3f8                	sw	a4,68(a5)
500006ae:	47d0                	lw	a2,12(a5)
500006b0:	05c2                	slli	a1,a1,0x10
500006b2:	81c1                	srli	a1,a1,0x10
500006b4:	c432                	sw	a2,8(sp)
500006b6:	00b11423          	sh	a1,8(sp)
500006ba:	4622                	lw	a2,8(sp)
500006bc:	c7d0                	sw	a2,12(a5)
500006be:	4790                	lw	a2,8(a5)
500006c0:	c632                	sw	a2,12(sp)
500006c2:	c62a                	sw	a0,12(sp)
500006c4:	4732                	lw	a4,12(sp)
500006c6:	c798                	sw	a4,8(a5)
500006c8:	47d8                	lw	a4,12(a5)
500006ca:	8f55                	or	a4,a4,a3
500006cc:	c7d8                	sw	a4,12(a5)
500006ce:	0141                	addi	sp,sp,16
500006d0:	8082                	ret

500006d2 <ulp_lp_core_lp_timer_get_cycle_count>:
500006d2:	600b17b7          	lui	a5,0x600b1
500006d6:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
500006da:	4b98                	lw	a4,16(a5)
500006dc:	100006b7          	lui	a3,0x10000
500006e0:	1101                	addi	sp,sp,-32
500006e2:	8f55                	or	a4,a4,a3
500006e4:	cb98                	sw	a4,16(a5)
500006e6:	4bd8                	lw	a4,20(a5)
500006e8:	ce3a                	sw	a4,28(sp)
500006ea:	4572                	lw	a0,28(sp)
500006ec:	4f9c                	lw	a5,24(a5)
500006ee:	cc3e                	sw	a5,24(sp)
500006f0:	01815583          	lhu	a1,24(sp)
500006f4:	6105                	addi	sp,sp,32
500006f6:	8082                	ret

500006f8 <ulp_lp_core_lp_timer_set_wakeup_time>:
500006f8:	1101                	addi	sp,sp,-32
500006fa:	ce06                	sw	ra,28(sp)
500006fc:	cc22                	sw	s0,24(sp)
500006fe:	ca26                	sw	s1,20(sp)
50000700:	c84a                	sw	s2,16(sp)
50000702:	c64e                	sw	s3,12(sp)
50000704:	892a                	mv	s2,a0
50000706:	84ae                	mv	s1,a1
50000708:	37e9                	jal	500006d2 <ulp_lp_core_lp_timer_get_cycle_count>
5000070a:	600b17b7          	lui	a5,0x600b1
5000070e:	43d0                	lw	a2,4(a5)
50000710:	89ae                	mv	s3,a1
50000712:	04ce                	slli	s1,s1,0x13
50000714:	00d95593          	srli	a1,s2,0xd
50000718:	842a                	mv	s0,a0
5000071a:	8dc5                	or	a1,a1,s1
5000071c:	01391513          	slli	a0,s2,0x13
50000720:	4681                	li	a3,0
50000722:	3af1                	jal	500000fe <__udivdi3>
50000724:	87aa                	mv	a5,a0
50000726:	9522                	add	a0,a0,s0
50000728:	4462                	lw	s0,24(sp)
5000072a:	40f2                	lw	ra,28(sp)
5000072c:	44d2                	lw	s1,20(sp)
5000072e:	4942                	lw	s2,16(sp)
50000730:	95ce                	add	a1,a1,s3
50000732:	49b2                	lw	s3,12(sp)
50000734:	00f537b3          	sltu	a5,a0,a5
50000738:	95be                	add	a1,a1,a5
5000073a:	6105                	addi	sp,sp,32
5000073c:	bfb9                	j	5000069a <lp_timer_hal_set_alarm_target>

5000073e <ulp_lp_core_lp_timer_set_wakeup_ticks>:
5000073e:	1141                	addi	sp,sp,-16
50000740:	c422                	sw	s0,8(sp)
50000742:	c226                	sw	s1,4(sp)
50000744:	c606                	sw	ra,12(sp)
50000746:	842a                	mv	s0,a0
50000748:	84ae                	mv	s1,a1
5000074a:	3761                	jal	500006d2 <ulp_lp_core_lp_timer_get_cycle_count>
5000074c:	87aa                	mv	a5,a0
5000074e:	9522                	add	a0,a0,s0
50000750:	4422                	lw	s0,8(sp)
50000752:	40b2                	lw	ra,12(sp)
50000754:	95a6                	add	a1,a1,s1
50000756:	4492                	lw	s1,4(sp)
50000758:	00f537b3          	sltu	a5,a0,a5
5000075c:	95be                	add	a1,a1,a5
5000075e:	0141                	addi	sp,sp,16
50000760:	bf2d                	j	5000069a <lp_timer_hal_set_alarm_target>

50000762 <ulp_lp_core_lp_timer_disable>:
50000762:	600b17b7          	lui	a5,0x600b1
50000766:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
5000076a:	47d8                	lw	a4,12(a5)
5000076c:	800006b7          	lui	a3,0x80000
50000770:	fff68613          	addi	a2,a3,-1 # 7fffffff <LPPERI+0x1ff4d7ff>
50000774:	8f71                	and	a4,a4,a2
50000776:	c7d8                	sw	a4,12(a5)
50000778:	43f8                	lw	a4,68(a5)
5000077a:	8f55                	or	a4,a4,a3
5000077c:	c3f8                	sw	a4,68(a5)
5000077e:	8082                	ret

50000780 <lp_core_startup>:
50000780:	1141                	addi	sp,sp,-16
50000782:	c606                	sw	ra,12(sp)
50000784:	2819                	jal	5000079a <ulp_lp_core_update_wakeup_cause>
50000786:	3be1                	jal	5000055e <main>
50000788:	3721                	jal	50000690 <ulp_lp_core_memory_shared_cfg_get>
5000078a:	87aa                	mv	a5,a0
5000078c:	47cc                	lw	a1,12(a5)
5000078e:	4508                	lw	a0,8(a0)
50000790:	00b567b3          	or	a5,a0,a1
50000794:	c391                	beqz	a5,50000798 <lp_core_startup+0x18>
50000796:	3765                	jal	5000073e <ulp_lp_core_lp_timer_set_wakeup_ticks>
50000798:	22ad                	jal	50000902 <ulp_lp_core_halt>

5000079a <ulp_lp_core_update_wakeup_cause>:
5000079a:	600b0737          	lui	a4,0x600b0
5000079e:	00070693          	mv	a3,a4
500007a2:	1806d683          	lhu	a3,384(a3)
500007a6:	500017b7          	lui	a5,0x50001
500007aa:	f207a823          	sw	zero,-208(a5) # 50000f30 <lp_wakeup_cause>
500007ae:	8a85                	andi	a3,a3,1
500007b0:	00070713          	mv	a4,a4
500007b4:	ce81                	beqz	a3,500007cc <ulp_lp_core_update_wakeup_cause+0x32>
500007b6:	16c72683          	lw	a3,364(a4) # 600b016c <PMU+0x16c>
500007ba:	0006d963          	bgez	a3,500007cc <ulp_lp_core_update_wakeup_cause+0x32>
500007be:	4685                	li	a3,1
500007c0:	f2d7a823          	sw	a3,-208(a5)
500007c4:	800006b7          	lui	a3,0x80000
500007c8:	16d72c23          	sw	a3,376(a4)
500007cc:	18075683          	lhu	a3,384(a4)
500007d0:	8a89                	andi	a3,a3,2
500007d2:	c29d                	beqz	a3,500007f8 <ulp_lp_core_update_wakeup_cause+0x5e>
500007d4:	600b16b7          	lui	a3,0x600b1
500007d8:	40068693          	addi	a3,a3,1024 # 600b1400 <LP_UART>
500007dc:	42d0                	lw	a2,4(a3)
500007de:	00c61593          	slli	a1,a2,0xc
500007e2:	0005db63          	bgez	a1,500007f8 <ulp_lp_core_update_wakeup_cause+0x5e>
500007e6:	f307a603          	lw	a2,-208(a5)
500007ea:	00266613          	ori	a2,a2,2
500007ee:	f2c7a823          	sw	a2,-208(a5)
500007f2:	00080637          	lui	a2,0x80
500007f6:	ca90                	sw	a2,16(a3)
500007f8:	18075683          	lhu	a3,384(a4)
500007fc:	8a91                	andi	a3,a3,4
500007fe:	c6c1                	beqz	a3,50000886 <ulp_lp_core_update_wakeup_cause+0xec>
50000800:	600b26b7          	lui	a3,0x600b2
50000804:	00068693          	mv	a3,a3
50000808:	4e90                	lw	a2,24(a3)
5000080a:	1141                	addi	sp,sp,-16
5000080c:	c432                	sw	a2,8(sp)
5000080e:	00814603          	lbu	a2,8(sp)
50000812:	ce11                	beqz	a2,5000082e <ulp_lp_core_update_wakeup_cause+0x94>
50000814:	f307a603          	lw	a2,-208(a5)
50000818:	00466613          	ori	a2,a2,4
5000081c:	f2c7a823          	sw	a2,-208(a5)
50000820:	5290                	lw	a2,32(a3)
50000822:	c632                	sw	a2,12(sp)
50000824:	567d                	li	a2,-1
50000826:	00c10623          	sb	a2,12(sp)
5000082a:	4632                	lw	a2,12(sp)
5000082c:	d290                	sw	a2,32(a3)
5000082e:	18075683          	lhu	a3,384(a4)
50000832:	8aa1                	andi	a3,a3,8
50000834:	c29d                	beqz	a3,5000085a <ulp_lp_core_update_wakeup_cause+0xc0>
50000836:	600b16b7          	lui	a3,0x600b1
5000083a:	00068693          	mv	a3,a3
5000083e:	4ab0                	lw	a2,80(a3)
50000840:	8205                	srli	a2,a2,0x1
50000842:	8a05                	andi	a2,a2,1
50000844:	ca19                	beqz	a2,5000085a <ulp_lp_core_update_wakeup_cause+0xc0>
50000846:	f307a603          	lw	a2,-208(a5)
5000084a:	00866613          	ori	a2,a2,8
5000084e:	f2c7a823          	sw	a2,-208(a5)
50000852:	4ab0                	lw	a2,80(a3)
50000854:	00166613          	ori	a2,a2,1
50000858:	cab0                	sw	a2,80(a3)
5000085a:	18075703          	lhu	a4,384(a4)
5000085e:	8b41                	andi	a4,a4,16
50000860:	c30d                	beqz	a4,50000882 <ulp_lp_core_update_wakeup_cause+0xe8>
50000862:	600b1737          	lui	a4,0x600b1
50000866:	c0070713          	addi	a4,a4,-1024 # 600b0c00 <LP_TIMER>
5000086a:	5f14                	lw	a3,56(a4)
5000086c:	0006db63          	bgez	a3,50000882 <ulp_lp_core_update_wakeup_cause+0xe8>
50000870:	f307a683          	lw	a3,-208(a5)
50000874:	0106e693          	ori	a3,a3,16
50000878:	f2d7a823          	sw	a3,-208(a5)
5000087c:	800007b7          	lui	a5,0x80000
50000880:	c37c                	sw	a5,68(a4)
50000882:	0141                	addi	sp,sp,16
50000884:	8082                	ret
50000886:	18075683          	lhu	a3,384(a4)
5000088a:	8aa1                	andi	a3,a3,8
5000088c:	c29d                	beqz	a3,500008b2 <ulp_lp_core_update_wakeup_cause+0x118>
5000088e:	600b16b7          	lui	a3,0x600b1
50000892:	00068693          	mv	a3,a3
50000896:	4ab0                	lw	a2,80(a3)
50000898:	8205                	srli	a2,a2,0x1
5000089a:	8a05                	andi	a2,a2,1
5000089c:	ca19                	beqz	a2,500008b2 <ulp_lp_core_update_wakeup_cause+0x118>
5000089e:	f307a603          	lw	a2,-208(a5) # 7fffff30 <LPPERI+0x1ff4d730>
500008a2:	00866613          	ori	a2,a2,8
500008a6:	f2c7a823          	sw	a2,-208(a5)
500008aa:	4ab0                	lw	a2,80(a3)
500008ac:	00166613          	ori	a2,a2,1
500008b0:	cab0                	sw	a2,80(a3)
500008b2:	18075703          	lhu	a4,384(a4)
500008b6:	8b41                	andi	a4,a4,16
500008b8:	c315                	beqz	a4,500008dc <ulp_lp_core_update_wakeup_cause+0x142>
500008ba:	600b1737          	lui	a4,0x600b1
500008be:	c0070713          	addi	a4,a4,-1024 # 600b0c00 <LP_TIMER>
500008c2:	5f14                	lw	a3,56(a4)
500008c4:	0006dc63          	bgez	a3,500008dc <ulp_lp_core_update_wakeup_cause+0x142>
500008c8:	f307a683          	lw	a3,-208(a5)
500008cc:	0106e693          	ori	a3,a3,16
500008d0:	f2d7a823          	sw	a3,-208(a5)
500008d4:	800007b7          	lui	a5,0x80000
500008d8:	c37c                	sw	a5,68(a4)
500008da:	8082                	ret
500008dc:	8082                	ret

500008de <ulp_lp_core_wakeup_main_processor>:
500008de:	600b07b7          	lui	a5,0x600b0
500008e2:	18478793          	addi	a5,a5,388 # 600b0184 <PMU+0x184>
500008e6:	4398                	lw	a4,0(a5)
500008e8:	400006b7          	lui	a3,0x40000
500008ec:	8f55                	or	a4,a4,a3
500008ee:	c398                	sw	a4,0(a5)
500008f0:	8082                	ret

500008f2 <ulp_lp_core_delay_cycles>:
500008f2:	b0002773          	csrr	a4,mcycle
500008f6:	b00027f3          	csrr	a5,mcycle
500008fa:	8f99                	sub	a5,a5,a4
500008fc:	fea7ede3          	bltu	a5,a0,500008f6 <ulp_lp_core_delay_cycles+0x4>
50000900:	8082                	ret

50000902 <ulp_lp_core_halt>:
50000902:	600b07b7          	lui	a5,0x600b0
50000906:	00078793          	mv	a5,a5
5000090a:	1807a703          	lw	a4,384(a5) # 600b0180 <PMU+0x180>
5000090e:	800006b7          	lui	a3,0x80000
50000912:	8f55                	or	a4,a4,a3
50000914:	18e7a023          	sw	a4,384(a5)
50000918:	a001                	j	50000918 <ulp_lp_core_halt+0x16>

5000091a <ulp_lp_core_stop_lp_core>:
5000091a:	600b07b7          	lui	a5,0x600b0
5000091e:	00078793          	mv	a5,a5
50000922:	18079023          	sh	zero,384(a5) # 600b0180 <PMU+0x180>
50000926:	1807a703          	lw	a4,384(a5)
5000092a:	800006b7          	lui	a3,0x80000
5000092e:	8f55                	or	a4,a4,a3
50000930:	18e7a023          	sw	a4,384(a5)
50000934:	8082                	ret

50000936 <ulp_lp_core_abort>:
50000936:	1141                	addi	sp,sp,-16
50000938:	c606                	sw	ra,12(sp)
5000093a:	37c5                	jal	5000091a <ulp_lp_core_stop_lp_core>
5000093c:	a001                	j	5000093c <ulp_lp_core_abort+0x6>

5000093e <ulp_lp_core_lp_timer_intr_clear>:
5000093e:	600b17b7          	lui	a5,0x600b1
50000942:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
50000946:	43f8                	lw	a4,68(a5)
50000948:	800006b7          	lui	a3,0x80000
5000094c:	8f55                	or	a4,a4,a3
5000094e:	c3f8                	sw	a4,68(a5)
50000950:	8082                	ret

50000952 <ulp_lp_core_efuse_intr_handler>:
50000952:	1141                	addi	sp,sp,-16
50000954:	c606                	sw	ra,12(sp)
50000956:	37c5                	jal	50000936 <ulp_lp_core_abort>

50000958 <ulp_lp_core_panic_handler>:
50000958:	1141                	addi	sp,sp,-16
5000095a:	c606                	sw	ra,12(sp)
5000095c:	3fe9                	jal	50000936 <ulp_lp_core_abort>

5000095e <ulp_lp_core_intr_handler>:
5000095e:	600b37b7          	lui	a5,0x600b3
50000962:	1101                	addi	sp,sp,-32
50000964:	80078793          	addi	a5,a5,-2048 # 600b2800 <LPPERI>
50000968:	c84a                	sw	s2,16(sp)
5000096a:	0207a903          	lw	s2,32(a5)
5000096e:	cc22                	sw	s0,24(sp)
50000970:	50001437          	lui	s0,0x50001
50000974:	ca26                	sw	s1,20(sp)
50000976:	c64e                	sw	s3,12(sp)
50000978:	ce06                	sw	ra,28(sp)
5000097a:	03f97913          	andi	s2,s2,63
5000097e:	e1440413          	addi	s0,s0,-492 # 50000e14 <s_intr_handlers>
50000982:	4481                	li	s1,0
50000984:	4999                	li	s3,6
50000986:	409957b3          	sra	a5,s2,s1
5000098a:	8b85                	andi	a5,a5,1
5000098c:	c781                	beqz	a5,50000994 <ulp_lp_core_intr_handler+0x36>
5000098e:	401c                	lw	a5,0(s0)
50000990:	c391                	beqz	a5,50000994 <ulp_lp_core_intr_handler+0x36>
50000992:	9782                	jalr	a5
50000994:	0485                	addi	s1,s1,1
50000996:	0411                	addi	s0,s0,4
50000998:	ff3497e3          	bne	s1,s3,50000986 <ulp_lp_core_intr_handler+0x28>
5000099c:	40f2                	lw	ra,28(sp)
5000099e:	4462                	lw	s0,24(sp)
500009a0:	44d2                	lw	s1,20(sp)
500009a2:	4942                	lw	s2,16(sp)
500009a4:	49b2                	lw	s3,12(sp)
500009a6:	6105                	addi	sp,sp,32
500009a8:	8082                	ret

500009aa <lp_core_i2c_wait_for_interrupt>:
500009aa:	1101                	addi	sp,sp,-32
500009ac:	cc22                	sw	s0,24(sp)
500009ae:	ca26                	sw	s1,20(sp)
500009b0:	c84a                	sw	s2,16(sp)
500009b2:	c64e                	sw	s3,12(sp)
500009b4:	ce06                	sw	ra,28(sp)
500009b6:	842a                	mv	s0,a0
500009b8:	84ae                	mv	s1,a1
500009ba:	4901                	li	s2,0
500009bc:	500019b7          	lui	s3,0x50001
500009c0:	e349a783          	lw	a5,-460(s3) # 50000e34 <dev>
500009c4:	57d8                	lw	a4,44(a5)
500009c6:	00e476b3          	and	a3,s0,a4
500009ca:	ca8d                	beqz	a3,500009fc <lp_core_i2c_wait_for_interrupt+0x52>
500009cc:	40077693          	andi	a3,a4,1024
500009d0:	ca99                	beqz	a3,500009e6 <lp_core_i2c_wait_for_interrupt+0x3c>
500009d2:	d3c0                	sw	s0,36(a5)
500009d4:	10800513          	li	a0,264
500009d8:	40f2                	lw	ra,28(sp)
500009da:	4462                	lw	s0,24(sp)
500009dc:	44d2                	lw	s1,20(sp)
500009de:	4942                	lw	s2,16(sp)
500009e0:	49b2                	lw	s3,12(sp)
500009e2:	6105                	addi	sp,sp,32
500009e4:	8082                	ret
500009e6:	08077713          	andi	a4,a4,128
500009ea:	c711                	beqz	a4,500009f6 <lp_core_i2c_wait_for_interrupt+0x4c>
500009ec:	5794                	lw	a3,40(a5)
500009ee:	fff44713          	not	a4,s0
500009f2:	8f75                	and	a4,a4,a3
500009f4:	d798                	sw	a4,40(a5)
500009f6:	d3c0                	sw	s0,36(a5)
500009f8:	4501                	li	a0,0
500009fa:	bff9                	j	500009d8 <lp_core_i2c_wait_for_interrupt+0x2e>
500009fc:	fc04c4e3          	bltz	s1,500009c4 <lp_core_i2c_wait_for_interrupt+0x1a>
50000a00:	4505                	li	a0,1
50000a02:	0905                	addi	s2,s2,1
50000a04:	35fd                	jal	500008f2 <ulp_lp_core_delay_cycles>
50000a06:	fa996de3          	bltu	s2,s1,500009c0 <lp_core_i2c_wait_for_interrupt+0x16>
50000a0a:	e349a783          	lw	a5,-460(s3)
50000a0e:	fff44713          	not	a4,s0
50000a12:	10700513          	li	a0,263
50000a16:	5794                	lw	a3,40(a5)
50000a18:	8f75                	and	a4,a4,a3
50000a1a:	d798                	sw	a4,40(a5)
50000a1c:	d3c0                	sw	s0,36(a5)
50000a1e:	bf6d                	j	500009d8 <lp_core_i2c_wait_for_interrupt+0x2e>

50000a20 <lp_core_i2c_format_cmd.constprop.0>:
50000a20:	47fd                	li	a5,31
50000a22:	02a7e263          	bltu	a5,a0,50000a46 <lp_core_i2c_format_cmd.constprop.0+0x26>
50000a26:	060a                	slli	a2,a2,0x2
50000a28:	500017b7          	lui	a5,0x50001
50000a2c:	8ed1                	or	a3,a3,a2
50000a2e:	058e                	slli	a1,a1,0x3
50000a30:	e347a783          	lw	a5,-460(a5) # 50000e34 <dev>
50000a34:	8ecd                	or	a3,a3,a1
50000a36:	03d6f693          	andi	a3,a3,61
50000a3a:	0551                	addi	a0,a0,20
50000a3c:	06a2                	slli	a3,a3,0x8
50000a3e:	050a                	slli	a0,a0,0x2
50000a40:	8f55                	or	a4,a4,a3
50000a42:	97aa                	add	a5,a5,a0
50000a44:	c798                	sw	a4,8(a5)
50000a46:	8082                	ret

50000a48 <lp_core_i2c_config_device_addr.constprop.0>:
50000a48:	500017b7          	lui	a5,0x50001
50000a4c:	e347a703          	lw	a4,-460(a5) # 50000e34 <dev>
50000a50:	500017b7          	lui	a5,0x50001
50000a54:	e307c783          	lbu	a5,-464(a5) # 50000e30 <s_ack_check_en>
50000a58:	0506                	slli	a0,a0,0x1
50000a5a:	8d4d                	or	a0,a0,a1
50000a5c:	0087e793          	ori	a5,a5,8
50000a60:	0397f793          	andi	a5,a5,57
50000a64:	0ff57513          	zext.b	a0,a0
50000a68:	07a2                	slli	a5,a5,0x8
50000a6a:	cf48                	sw	a0,28(a4)
50000a6c:	0017e793          	ori	a5,a5,1
50000a70:	cf7c                	sw	a5,92(a4)
50000a72:	4785                	li	a5,1
50000a74:	00f60023          	sb	a5,0(a2) # 80000 <RvExcFrameSize+0x7ff6c>
50000a78:	8082                	ret

50000a7a <lp_core_i2c_master_read_from_device>:
50000a7a:	711d                	addi	sp,sp,-96
50000a7c:	ce86                	sw	ra,92(sp)
50000a7e:	cca2                	sw	s0,88(sp)
50000a80:	caa6                	sw	s1,84(sp)
50000a82:	c8ca                	sw	s2,80(sp)
50000a84:	c6ce                	sw	s3,76(sp)
50000a86:	c4d2                	sw	s4,72(sp)
50000a88:	c2d6                	sw	s5,68(sp)
50000a8a:	c0da                	sw	s6,64(sp)
50000a8c:	de5e                	sw	s7,60(sp)
50000a8e:	dc62                	sw	s8,56(sp)
50000a90:	da66                	sw	s9,52(sp)
50000a92:	d86a                	sw	s10,48(sp)
50000a94:	d66e                	sw	s11,44(sp)
50000a96:	c63a                	sw	a4,12(sp)
50000a98:	e28d                	bnez	a3,50000aba <lp_core_i2c_master_read_from_device+0x40>
50000a9a:	4501                	li	a0,0
50000a9c:	40f6                	lw	ra,92(sp)
50000a9e:	4466                	lw	s0,88(sp)
50000aa0:	44d6                	lw	s1,84(sp)
50000aa2:	4946                	lw	s2,80(sp)
50000aa4:	49b6                	lw	s3,76(sp)
50000aa6:	4a26                	lw	s4,72(sp)
50000aa8:	4a96                	lw	s5,68(sp)
50000aaa:	4b06                	lw	s6,64(sp)
50000aac:	5bf2                	lw	s7,60(sp)
50000aae:	5c62                	lw	s8,56(sp)
50000ab0:	5cd2                	lw	s9,52(sp)
50000ab2:	5d42                	lw	s10,48(sp)
50000ab4:	5db2                	lw	s11,44(sp)
50000ab6:	6125                	addi	sp,sp,96
50000ab8:	8082                	ret
50000aba:	0ff00713          	li	a4,255
50000abe:	8436                	mv	s0,a3
50000ac0:	10400513          	li	a0,260
50000ac4:	fcd76ce3          	bltu	a4,a3,50000a9c <lp_core_i2c_master_read_from_device+0x22>
50000ac8:	50001b37          	lui	s6,0x50001
50000acc:	e34b2483          	lw	s1,-460(s6) # 50000e34 <dev>
50000ad0:	87ae                	mv	a5,a1
50000ad2:	670d                	lui	a4,0x3
50000ad4:	ccb8                	sw	a4,88(s1)
50000ad6:	853e                	mv	a0,a5
50000ad8:	8a32                	mv	s4,a2
50000ada:	4585                	li	a1,1
50000adc:	01b10613          	addi	a2,sp,27
50000ae0:	37a5                	jal	50000a48 <lp_core_i2c_config_device_addr.constprop.0>
50000ae2:	549c                	lw	a5,40(s1)
50000ae4:	6c05                	lui	s8,0x1
50000ae6:	4981                	li	s3,0
50000ae8:	0887e793          	ori	a5,a5,136
50000aec:	d49c                	sw	a5,40(s1)
50000aee:	4d41                	li	s10,16
50000af0:	4489                	li	s1,2
50000af2:	4b85                	li	s7,1
50000af4:	800c0c13          	addi	s8,s8,-2048 # 800 <RvExcFrameSize+0x76c>
50000af8:	8922                	mv	s2,s0
50000afa:	008d5363          	bge	s10,s0,50000b00 <lp_core_i2c_master_read_from_device+0x86>
50000afe:	4941                	li	s2,16
50000b00:	41240cb3          	sub	s9,s0,s2
50000b04:	87a2                	mv	a5,s0
50000b06:	00148d93          	addi	s11,s1,1
50000b0a:	8466                	mv	s0,s9
50000b0c:	07779063          	bne	a5,s7,50000b6c <lp_core_i2c_master_read_from_device+0xf2>
50000b10:	8526                	mv	a0,s1
50000b12:	875e                	mv	a4,s7
50000b14:	4681                	li	a3,0
50000b16:	865e                	mv	a2,s7
50000b18:	458d                	li	a1,3
50000b1a:	3719                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000b1c:	0489                	addi	s1,s1,2
50000b1e:	4701                	li	a4,0
50000b20:	4681                	li	a3,0
50000b22:	4601                	li	a2,0
50000b24:	4589                	li	a1,2
50000b26:	856e                	mv	a0,s11
50000b28:	3de5                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000b2a:	e34b2783          	lw	a5,-460(s6)
50000b2e:	08800513          	li	a0,136
50000b32:	43d8                	lw	a4,4(a5)
50000b34:	01876733          	or	a4,a4,s8
50000b38:	c3d8                	sw	a4,4(a5)
50000b3a:	43d8                	lw	a4,4(a5)
50000b3c:	02076713          	ori	a4,a4,32
50000b40:	c3d8                	sw	a4,4(a5)
50000b42:	45b2                	lw	a1,12(sp)
50000b44:	359d                	jal	500009aa <lp_core_i2c_wait_for_interrupt>
50000b46:	f939                	bnez	a0,50000a9c <lp_core_i2c_master_read_from_device+0x22>
50000b48:	e34b2683          	lw	a3,-460(s6)
50000b4c:	4edc                	lw	a5,28(a3)
50000b4e:	ce3e                	sw	a5,28(sp)
50000b50:	01c14703          	lbu	a4,28(sp)
50000b54:	013507b3          	add	a5,a0,s3
50000b58:	97d2                	add	a5,a5,s4
50000b5a:	00e78023          	sb	a4,0(a5)
50000b5e:	0505                	addi	a0,a0,1
50000b60:	fea916e3          	bne	s2,a0,50000b4c <lp_core_i2c_master_read_from_device+0xd2>
50000b64:	99ca                	add	s3,s3,s2
50000b66:	f99049e3          	bgtz	s9,50000af8 <lp_core_i2c_master_read_from_device+0x7e>
50000b6a:	bf05                	j	50000a9a <lp_core_i2c_master_read_from_device+0x20>
50000b6c:	0ff97713          	zext.b	a4,s2
50000b70:	020c9963          	bnez	s9,50000ba2 <lp_core_i2c_master_read_from_device+0x128>
50000b74:	177d                	addi	a4,a4,-1 # 2fff <RvExcFrameSize+0x2f6b>
50000b76:	8526                	mv	a0,s1
50000b78:	0ff77713          	zext.b	a4,a4
50000b7c:	4681                	li	a3,0
50000b7e:	4601                	li	a2,0
50000b80:	458d                	li	a1,3
50000b82:	3d79                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000b84:	4705                	li	a4,1
50000b86:	863a                	mv	a2,a4
50000b88:	4681                	li	a3,0
50000b8a:	458d                	li	a1,3
50000b8c:	856e                	mv	a0,s11
50000b8e:	3d49                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000b90:	00248a93          	addi	s5,s1,2
50000b94:	4701                	li	a4,0
50000b96:	4681                	li	a3,0
50000b98:	4601                	li	a2,0
50000b9a:	4589                	li	a1,2
50000b9c:	8556                	mv	a0,s5
50000b9e:	048d                	addi	s1,s1,3
50000ba0:	b761                	j	50000b28 <lp_core_i2c_master_read_from_device+0xae>
50000ba2:	8526                	mv	a0,s1
50000ba4:	4681                	li	a3,0
50000ba6:	4601                	li	a2,0
50000ba8:	458d                	li	a1,3
50000baa:	3d9d                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000bac:	4701                	li	a4,0
50000bae:	4681                	li	a3,0
50000bb0:	4601                	li	a2,0
50000bb2:	4591                	li	a1,4
50000bb4:	856e                	mv	a0,s11
50000bb6:	35ad                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000bb8:	4481                	li	s1,0
50000bba:	bf85                	j	50000b2a <lp_core_i2c_master_read_from_device+0xb0>

50000bbc <lp_core_i2c_master_write_to_device>:
50000bbc:	14068963          	beqz	a3,50000d0e <lp_core_i2c_master_write_to_device+0x152>
50000bc0:	715d                	addi	sp,sp,-80
50000bc2:	c0ca                	sw	s2,64(sp)
50000bc4:	c686                	sw	ra,76(sp)
50000bc6:	c4a2                	sw	s0,72(sp)
50000bc8:	c2a6                	sw	s1,68(sp)
50000bca:	de4e                	sw	s3,60(sp)
50000bcc:	dc52                	sw	s4,56(sp)
50000bce:	da56                	sw	s5,52(sp)
50000bd0:	d85a                	sw	s6,48(sp)
50000bd2:	d65e                	sw	s7,44(sp)
50000bd4:	d462                	sw	s8,40(sp)
50000bd6:	d266                	sw	s9,36(sp)
50000bd8:	d06a                	sw	s10,32(sp)
50000bda:	ce6e                	sw	s11,28(sp)
50000bdc:	0ff00793          	li	a5,255
50000be0:	8936                	mv	s2,a3
50000be2:	10400513          	li	a0,260
50000be6:	0ed7e463          	bltu	a5,a3,50000cce <lp_core_i2c_master_write_to_device+0x112>
50000bea:	50001bb7          	lui	s7,0x50001
50000bee:	e34ba403          	lw	s0,-460(s7) # 50000e34 <dev>
50000bf2:	882e                	mv	a6,a1
50000bf4:	8ab2                	mv	s5,a2
50000bf6:	441c                	lw	a5,8(s0)
50000bf8:	8b3a                	mv	s6,a4
50000bfa:	8391                	srli	a5,a5,0x4
50000bfc:	8b85                	andi	a5,a5,1
50000bfe:	c789                	beqz	a5,50000c08 <lp_core_i2c_master_write_to_device+0x4c>
50000c00:	405c                	lw	a5,4(s0)
50000c02:	4007e793          	ori	a5,a5,1024
50000c06:	c05c                	sw	a5,4(s0)
50000c08:	4c1c                	lw	a5,24(s0)
50000c0a:	6709                	lui	a4,0x2
50000c0c:	00f10613          	addi	a2,sp,15
50000c10:	8fd9                	or	a5,a5,a4
50000c12:	cc1c                	sw	a5,24(s0)
50000c14:	4c1c                	lw	a5,24(s0)
50000c16:	7779                	lui	a4,0xffffe
50000c18:	177d                	addi	a4,a4,-1 # ffffdfff <LPPERI+0x9ff4b7ff>
50000c1a:	8ff9                	and	a5,a5,a4
50000c1c:	cc1c                	sw	a5,24(s0)
50000c1e:	4c1c                	lw	a5,24(s0)
50000c20:	6705                	lui	a4,0x1
50000c22:	4581                	li	a1,0
50000c24:	8fd9                	or	a5,a5,a4
50000c26:	cc1c                	sw	a5,24(s0)
50000c28:	4c1c                	lw	a5,24(s0)
50000c2a:	777d                	lui	a4,0xfffff
50000c2c:	177d                	addi	a4,a4,-1 # ffffefff <LPPERI+0x9ff4c7ff>
50000c2e:	8ff9                	and	a5,a5,a4
50000c30:	cc1c                	sw	a5,24(s0)
50000c32:	678d                	lui	a5,0x3
50000c34:	cc3c                	sw	a5,88(s0)
50000c36:	8542                	mv	a0,a6
50000c38:	50001c37          	lui	s8,0x50001
50000c3c:	3531                	jal	50000a48 <lp_core_i2c_config_device_addr.constprop.0>
50000c3e:	e30c4483          	lbu	s1,-464(s8) # 50000e30 <s_ack_check_en>
50000c42:	541c                	lw	a5,40(s0)
50000c44:	00f14703          	lbu	a4,15(sp)
50000c48:	009034b3          	snez	s1,s1
50000c4c:	04aa                	slli	s1,s1,0xa
50000c4e:	08848493          	addi	s1,s1,136
50000c52:	8fc5                	or	a5,a5,s1
50000c54:	d41c                	sw	a5,40(s0)
50000c56:	6c85                	lui	s9,0x1
50000c58:	47c1                	li	a5,16
50000c5a:	8f99                	sub	a5,a5,a4
50000c5c:	844a                	mv	s0,s2
50000c5e:	4a01                	li	s4,0
50000c60:	4989                	li	s3,2
50000c62:	800c8c93          	addi	s9,s9,-2048 # 800 <RvExcFrameSize+0x76c>
50000c66:	8922                	mv	s2,s0
50000c68:	0087f363          	bgeu	a5,s0,50000c6e <lp_core_i2c_master_write_to_device+0xb2>
50000c6c:	893e                	mv	s2,a5
50000c6e:	e34bad03          	lw	s10,-460(s7)
50000c72:	41240433          	sub	s0,s0,s2
50000c76:	4781                	li	a5,0
50000c78:	0ff97693          	zext.b	a3,s2
50000c7c:	06d7c863          	blt	a5,a3,50000cec <lp_core_i2c_master_write_to_device+0x130>
50000c80:	e30c4683          	lbu	a3,-464(s8)
50000c84:	0ff97713          	zext.b	a4,s2
50000c88:	4601                	li	a2,0
50000c8a:	4585                	li	a1,1
50000c8c:	854e                	mv	a0,s3
50000c8e:	00198d93          	addi	s11,s3,1
50000c92:	3379                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000c94:	e42d                	bnez	s0,50000cfe <lp_core_i2c_master_write_to_device+0x142>
50000c96:	4701                	li	a4,0
50000c98:	4681                	li	a3,0
50000c9a:	4601                	li	a2,0
50000c9c:	4589                	li	a1,2
50000c9e:	856e                	mv	a0,s11
50000ca0:	0989                	addi	s3,s3,2
50000ca2:	3bbd                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000ca4:	004d2783          	lw	a5,4(s10)
50000ca8:	85da                	mv	a1,s6
50000caa:	8526                	mv	a0,s1
50000cac:	0197e7b3          	or	a5,a5,s9
50000cb0:	00fd2223          	sw	a5,4(s10)
50000cb4:	004d2783          	lw	a5,4(s10)
50000cb8:	0207e793          	ori	a5,a5,32
50000cbc:	00fd2223          	sw	a5,4(s10)
50000cc0:	31ed                	jal	500009aa <lp_core_i2c_wait_for_interrupt>
50000cc2:	e511                	bnez	a0,50000cce <lp_core_i2c_master_write_to_device+0x112>
50000cc4:	9a4a                	add	s4,s4,s2
50000cc6:	47c1                	li	a5,16
50000cc8:	f8804fe3          	bgtz	s0,50000c66 <lp_core_i2c_master_write_to_device+0xaa>
50000ccc:	4501                	li	a0,0
50000cce:	40b6                	lw	ra,76(sp)
50000cd0:	4426                	lw	s0,72(sp)
50000cd2:	4496                	lw	s1,68(sp)
50000cd4:	4906                	lw	s2,64(sp)
50000cd6:	59f2                	lw	s3,60(sp)
50000cd8:	5a62                	lw	s4,56(sp)
50000cda:	5ad2                	lw	s5,52(sp)
50000cdc:	5b42                	lw	s6,48(sp)
50000cde:	5bb2                	lw	s7,44(sp)
50000ce0:	5c22                	lw	s8,40(sp)
50000ce2:	5c92                	lw	s9,36(sp)
50000ce4:	5d02                	lw	s10,32(sp)
50000ce6:	4df2                	lw	s11,28(sp)
50000ce8:	6161                	addi	sp,sp,80
50000cea:	8082                	ret
50000cec:	01478733          	add	a4,a5,s4
50000cf0:	9756                	add	a4,a4,s5
50000cf2:	00074703          	lbu	a4,0(a4)
50000cf6:	0785                	addi	a5,a5,1 # 3001 <RvExcFrameSize+0x2f6d>
50000cf8:	00ed2e23          	sw	a4,28(s10)
50000cfc:	b741                	j	50000c7c <lp_core_i2c_master_write_to_device+0xc0>
50000cfe:	4701                	li	a4,0
50000d00:	4681                	li	a3,0
50000d02:	4601                	li	a2,0
50000d04:	4591                	li	a1,4
50000d06:	856e                	mv	a0,s11
50000d08:	3b21                	jal	50000a20 <lp_core_i2c_format_cmd.constprop.0>
50000d0a:	4981                	li	s3,0
50000d0c:	bf61                	j	50000ca4 <lp_core_i2c_master_write_to_device+0xe8>
50000d0e:	4501                	li	a0,0
50000d10:	8082                	ret
