
.\build\esp-idf\main\ulp_core_main\ulp_core_main.elf:     file format elf32-littleriscv


Disassembly of section .vector.text:

50000000 <_vector_table>:
50000000:	6a80006f          	j	500006a8 <_panic_handler>
50000004:	6a40006f          	j	500006a8 <_panic_handler>
50000008:	6a00006f          	j	500006a8 <_panic_handler>
5000000c:	69c0006f          	j	500006a8 <_panic_handler>
50000010:	6980006f          	j	500006a8 <_panic_handler>
50000014:	6940006f          	j	500006a8 <_panic_handler>
50000018:	6900006f          	j	500006a8 <_panic_handler>
5000001c:	68c0006f          	j	500006a8 <_panic_handler>
50000020:	6880006f          	j	500006a8 <_panic_handler>
50000024:	6840006f          	j	500006a8 <_panic_handler>
50000028:	6800006f          	j	500006a8 <_panic_handler>
5000002c:	67c0006f          	j	500006a8 <_panic_handler>
50000030:	6780006f          	j	500006a8 <_panic_handler>
50000034:	6740006f          	j	500006a8 <_panic_handler>
50000038:	6700006f          	j	500006a8 <_panic_handler>
5000003c:	66c0006f          	j	500006a8 <_panic_handler>
50000040:	6680006f          	j	500006a8 <_panic_handler>
50000044:	6640006f          	j	500006a8 <_panic_handler>
50000048:	6600006f          	j	500006a8 <_panic_handler>
5000004c:	65c0006f          	j	500006a8 <_panic_handler>
50000050:	6580006f          	j	500006a8 <_panic_handler>
50000054:	6540006f          	j	500006a8 <_panic_handler>
50000058:	6500006f          	j	500006a8 <_panic_handler>
5000005c:	64c0006f          	j	500006a8 <_panic_handler>
50000060:	6480006f          	j	500006a8 <_panic_handler>
50000064:	6440006f          	j	500006a8 <_panic_handler>
50000068:	6400006f          	j	500006a8 <_panic_handler>
5000006c:	63c0006f          	j	500006a8 <_panic_handler>
50000070:	6380006f          	j	500006a8 <_panic_handler>
50000074:	6340006f          	j	500006a8 <_panic_handler>
50000078:	6a00006f          	j	50000718 <_interrupt_handler>
5000007c:	62c0006f          	j	500006a8 <_panic_handler>

Disassembly of section .text:

50000080 <reset_vector>:
50000080:	00000297          	auipc	t0,0x0
50000084:	f8028293          	addi	t0,t0,-128 # 50000000 <_vector_table>
50000088:	30529073          	csrw	mtvec,t0
5000008c:	a009                	j	5000008e <__start>

5000008e <__start>:
5000008e:	00001117          	auipc	sp,0x1
50000092:	f6210113          	addi	sp,sp,-158 # 50000ff0 <s_shared_mem>
50000096:	7fc000ef          	jal	50000892 <lp_core_startup>

5000009a <loop>:
5000009a:	a001                	j	5000009a <loop>

5000009c <sleep_store>:
5000009c:	7139                	addi	sp,sp,-64
5000009e:	c006                	sw	ra,0(sp)
500000a0:	c20e                	sw	gp,4(sp)
500000a2:	c412                	sw	tp,8(sp)
500000a4:	c822                	sw	s0,16(sp)
500000a6:	ca26                	sw	s1,20(sp)
500000a8:	cc4a                	sw	s2,24(sp)
500000aa:	ce4e                	sw	s3,28(sp)
500000ac:	d052                	sw	s4,32(sp)
500000ae:	d256                	sw	s5,36(sp)
500000b0:	036128a3          	sw	s6,49(sp)
500000b4:	d65e                	sw	s7,44(sp)
500000b6:	d862                	sw	s8,48(sp)
500000b8:	da66                	sw	s9,52(sp)
500000ba:	dc6a                	sw	s10,56(sp)
500000bc:	de6e                	sw	s11,60(sp)
500000be:	00001517          	auipc	a0,0x1
500000c2:	bf650513          	addi	a0,a0,-1034 # 50000cb4 <mrbc_sp_bottom>
500000c6:	00252023          	sw	sp,0(a0)
500000ca:	39c0006f          	j	50000466 <sleep_store2>

500000ce <sleep_restore>:
500000ce:	00001517          	auipc	a0,0x1
500000d2:	be650513          	addi	a0,a0,-1050 # 50000cb4 <mrbc_sp_bottom>
500000d6:	00052103          	lw	sp,0(a0)
500000da:	4082                	lw	ra,0(sp)
500000dc:	4192                	lw	gp,4(sp)
500000de:	4222                	lw	tp,8(sp)
500000e0:	4442                	lw	s0,16(sp)
500000e2:	44d2                	lw	s1,20(sp)
500000e4:	4962                	lw	s2,24(sp)
500000e6:	49f2                	lw	s3,28(sp)
500000e8:	5a02                	lw	s4,32(sp)
500000ea:	5a92                	lw	s5,36(sp)
500000ec:	03112b03          	lw	s6,49(sp)
500000f0:	5bb2                	lw	s7,44(sp)
500000f2:	5c42                	lw	s8,48(sp)
500000f4:	5cd2                	lw	s9,52(sp)
500000f6:	5d62                	lw	s10,56(sp)
500000f8:	5df2                	lw	s11,60(sp)
500000fa:	6121                	addi	sp,sp,64
500000fc:	00008067          	ret

50000100 <__udivdi3>:
50000100:	88aa                	mv	a7,a0
50000102:	832e                	mv	t1,a1
50000104:	8732                	mv	a4,a2
50000106:	882a                	mv	a6,a0
50000108:	87ae                	mv	a5,a1
5000010a:	20069663          	bnez	a3,50000316 <__udivdi3+0x216>
5000010e:	500015b7          	lui	a1,0x50001
50000112:	ad458593          	addi	a1,a1,-1324 # 50000ad4 <__clz_tab>
50000116:	0cc37163          	bgeu	t1,a2,500001d8 <__udivdi3+0xd8>
5000011a:	66c1                	lui	a3,0x10
5000011c:	0ad67763          	bgeu	a2,a3,500001ca <__udivdi3+0xca>
50000120:	10063693          	sltiu	a3,a2,256
50000124:	0016b693          	seqz	a3,a3
50000128:	068e                	slli	a3,a3,0x3
5000012a:	00d65533          	srl	a0,a2,a3
5000012e:	95aa                	add	a1,a1,a0
50000130:	0005c583          	lbu	a1,0(a1)
50000134:	02000513          	li	a0,32
50000138:	96ae                	add	a3,a3,a1
5000013a:	40d505b3          	sub	a1,a0,a3
5000013e:	00d50b63          	beq	a0,a3,50000154 <__udivdi3+0x54>
50000142:	00b317b3          	sll	a5,t1,a1
50000146:	00d8d6b3          	srl	a3,a7,a3
5000014a:	00b61733          	sll	a4,a2,a1
5000014e:	8fd5                	or	a5,a5,a3
50000150:	00b89833          	sll	a6,a7,a1
50000154:	01075593          	srli	a1,a4,0x10
50000158:	02b7d333          	divu	t1,a5,a1
5000015c:	01071613          	slli	a2,a4,0x10
50000160:	8241                	srli	a2,a2,0x10
50000162:	02b7f7b3          	remu	a5,a5,a1
50000166:	851a                	mv	a0,t1
50000168:	026608b3          	mul	a7,a2,t1
5000016c:	01079693          	slli	a3,a5,0x10
50000170:	01085793          	srli	a5,a6,0x10
50000174:	8fd5                	or	a5,a5,a3
50000176:	0117fc63          	bgeu	a5,a7,5000018e <__udivdi3+0x8e>
5000017a:	97ba                	add	a5,a5,a4
5000017c:	fff30513          	addi	a0,t1,-1
50000180:	00e7e763          	bltu	a5,a4,5000018e <__udivdi3+0x8e>
50000184:	0117f563          	bgeu	a5,a7,5000018e <__udivdi3+0x8e>
50000188:	ffe30513          	addi	a0,t1,-2
5000018c:	97ba                	add	a5,a5,a4
5000018e:	411787b3          	sub	a5,a5,a7
50000192:	02b7d8b3          	divu	a7,a5,a1
50000196:	0842                	slli	a6,a6,0x10
50000198:	01085813          	srli	a6,a6,0x10
5000019c:	02b7f7b3          	remu	a5,a5,a1
500001a0:	031606b3          	mul	a3,a2,a7
500001a4:	07c2                	slli	a5,a5,0x10
500001a6:	00f86833          	or	a6,a6,a5
500001aa:	87c6                	mv	a5,a7
500001ac:	00d87b63          	bgeu	a6,a3,500001c2 <__udivdi3+0xc2>
500001b0:	983a                	add	a6,a6,a4
500001b2:	fff88793          	addi	a5,a7,-1
500001b6:	00e86663          	bltu	a6,a4,500001c2 <__udivdi3+0xc2>
500001ba:	00d87463          	bgeu	a6,a3,500001c2 <__udivdi3+0xc2>
500001be:	ffe88793          	addi	a5,a7,-2
500001c2:	0542                	slli	a0,a0,0x10
500001c4:	8d5d                	or	a0,a0,a5
500001c6:	4581                	li	a1,0
500001c8:	8082                	ret
500001ca:	01000537          	lui	a0,0x1000
500001ce:	46e1                	li	a3,24
500001d0:	f4a67de3          	bgeu	a2,a0,5000012a <__udivdi3+0x2a>
500001d4:	46c1                	li	a3,16
500001d6:	bf91                	j	5000012a <__udivdi3+0x2a>
500001d8:	4681                	li	a3,0
500001da:	ca09                	beqz	a2,500001ec <__udivdi3+0xec>
500001dc:	67c1                	lui	a5,0x10
500001de:	08f67f63          	bgeu	a2,a5,5000027c <__udivdi3+0x17c>
500001e2:	10063693          	sltiu	a3,a2,256
500001e6:	0016b693          	seqz	a3,a3
500001ea:	068e                	slli	a3,a3,0x3
500001ec:	00d657b3          	srl	a5,a2,a3
500001f0:	95be                	add	a1,a1,a5
500001f2:	0005c783          	lbu	a5,0(a1)
500001f6:	97b6                	add	a5,a5,a3
500001f8:	02000693          	li	a3,32
500001fc:	40f685b3          	sub	a1,a3,a5
50000200:	08f69563          	bne	a3,a5,5000028a <__udivdi3+0x18a>
50000204:	40c307b3          	sub	a5,t1,a2
50000208:	4585                	li	a1,1
5000020a:	01075893          	srli	a7,a4,0x10
5000020e:	0317de33          	divu	t3,a5,a7
50000212:	01071613          	slli	a2,a4,0x10
50000216:	8241                	srli	a2,a2,0x10
50000218:	01085693          	srli	a3,a6,0x10
5000021c:	0317f7b3          	remu	a5,a5,a7
50000220:	8572                	mv	a0,t3
50000222:	03c60333          	mul	t1,a2,t3
50000226:	07c2                	slli	a5,a5,0x10
50000228:	8fd5                	or	a5,a5,a3
5000022a:	0067fc63          	bgeu	a5,t1,50000242 <__udivdi3+0x142>
5000022e:	97ba                	add	a5,a5,a4
50000230:	fffe0513          	addi	a0,t3,-1
50000234:	00e7e763          	bltu	a5,a4,50000242 <__udivdi3+0x142>
50000238:	0067f563          	bgeu	a5,t1,50000242 <__udivdi3+0x142>
5000023c:	ffee0513          	addi	a0,t3,-2
50000240:	97ba                	add	a5,a5,a4
50000242:	406787b3          	sub	a5,a5,t1
50000246:	0317d333          	divu	t1,a5,a7
5000024a:	0842                	slli	a6,a6,0x10
5000024c:	01085813          	srli	a6,a6,0x10
50000250:	0317f7b3          	remu	a5,a5,a7
50000254:	026606b3          	mul	a3,a2,t1
50000258:	07c2                	slli	a5,a5,0x10
5000025a:	00f86833          	or	a6,a6,a5
5000025e:	879a                	mv	a5,t1
50000260:	00d87b63          	bgeu	a6,a3,50000276 <__udivdi3+0x176>
50000264:	983a                	add	a6,a6,a4
50000266:	fff30793          	addi	a5,t1,-1
5000026a:	00e86663          	bltu	a6,a4,50000276 <__udivdi3+0x176>
5000026e:	00d87463          	bgeu	a6,a3,50000276 <__udivdi3+0x176>
50000272:	ffe30793          	addi	a5,t1,-2
50000276:	0542                	slli	a0,a0,0x10
50000278:	8d5d                	or	a0,a0,a5
5000027a:	8082                	ret
5000027c:	010007b7          	lui	a5,0x1000
50000280:	46e1                	li	a3,24
50000282:	f6f675e3          	bgeu	a2,a5,500001ec <__udivdi3+0xec>
50000286:	46c1                	li	a3,16
50000288:	b795                	j	500001ec <__udivdi3+0xec>
5000028a:	00b61733          	sll	a4,a2,a1
5000028e:	00f356b3          	srl	a3,t1,a5
50000292:	01075513          	srli	a0,a4,0x10
50000296:	00b31333          	sll	t1,t1,a1
5000029a:	00f8d7b3          	srl	a5,a7,a5
5000029e:	0067e7b3          	or	a5,a5,t1
500002a2:	02a6d333          	divu	t1,a3,a0
500002a6:	01071613          	slli	a2,a4,0x10
500002aa:	8241                	srli	a2,a2,0x10
500002ac:	00b89833          	sll	a6,a7,a1
500002b0:	02a6f6b3          	remu	a3,a3,a0
500002b4:	026608b3          	mul	a7,a2,t1
500002b8:	01069593          	slli	a1,a3,0x10
500002bc:	0107d693          	srli	a3,a5,0x10
500002c0:	8ecd                	or	a3,a3,a1
500002c2:	859a                	mv	a1,t1
500002c4:	0116fc63          	bgeu	a3,a7,500002dc <__udivdi3+0x1dc>
500002c8:	96ba                	add	a3,a3,a4
500002ca:	fff30593          	addi	a1,t1,-1
500002ce:	00e6e763          	bltu	a3,a4,500002dc <__udivdi3+0x1dc>
500002d2:	0116f563          	bgeu	a3,a7,500002dc <__udivdi3+0x1dc>
500002d6:	ffe30593          	addi	a1,t1,-2
500002da:	96ba                	add	a3,a3,a4
500002dc:	411686b3          	sub	a3,a3,a7
500002e0:	02a6d8b3          	divu	a7,a3,a0
500002e4:	07c2                	slli	a5,a5,0x10
500002e6:	83c1                	srli	a5,a5,0x10
500002e8:	02a6f6b3          	remu	a3,a3,a0
500002ec:	03160633          	mul	a2,a2,a7
500002f0:	06c2                	slli	a3,a3,0x10
500002f2:	8fd5                	or	a5,a5,a3
500002f4:	86c6                	mv	a3,a7
500002f6:	00c7fc63          	bgeu	a5,a2,5000030e <__udivdi3+0x20e>
500002fa:	97ba                	add	a5,a5,a4
500002fc:	fff88693          	addi	a3,a7,-1
50000300:	00e7e763          	bltu	a5,a4,5000030e <__udivdi3+0x20e>
50000304:	00c7f563          	bgeu	a5,a2,5000030e <__udivdi3+0x20e>
50000308:	ffe88693          	addi	a3,a7,-2
5000030c:	97ba                	add	a5,a5,a4
5000030e:	05c2                	slli	a1,a1,0x10
50000310:	8f91                	sub	a5,a5,a2
50000312:	8dd5                	or	a1,a1,a3
50000314:	bddd                	j	5000020a <__udivdi3+0x10a>
50000316:	12d5ef63          	bltu	a1,a3,50000454 <__udivdi3+0x354>
5000031a:	67c1                	lui	a5,0x10
5000031c:	02f6ff63          	bgeu	a3,a5,5000035a <__udivdi3+0x25a>
50000320:	1006b793          	sltiu	a5,a3,256
50000324:	0017b793          	seqz	a5,a5
50000328:	078e                	slli	a5,a5,0x3
5000032a:	50001737          	lui	a4,0x50001
5000032e:	00f6d5b3          	srl	a1,a3,a5
50000332:	ad470713          	addi	a4,a4,-1324 # 50000ad4 <__clz_tab>
50000336:	972e                	add	a4,a4,a1
50000338:	00074703          	lbu	a4,0(a4)
5000033c:	973e                	add	a4,a4,a5
5000033e:	02000793          	li	a5,32
50000342:	40e785b3          	sub	a1,a5,a4
50000346:	02e79163          	bne	a5,a4,50000368 <__udivdi3+0x268>
5000034a:	4505                	li	a0,1
5000034c:	e666eee3          	bltu	a3,t1,500001c8 <__udivdi3+0xc8>
50000350:	00c8b533          	sltu	a0,a7,a2
50000354:	00153513          	seqz	a0,a0
50000358:	8082                	ret
5000035a:	01000737          	lui	a4,0x1000
5000035e:	47e1                	li	a5,24
50000360:	fce6f5e3          	bgeu	a3,a4,5000032a <__udivdi3+0x22a>
50000364:	47c1                	li	a5,16
50000366:	b7d1                	j	5000032a <__udivdi3+0x22a>
50000368:	00e65533          	srl	a0,a2,a4
5000036c:	00b696b3          	sll	a3,a3,a1
50000370:	00e357b3          	srl	a5,t1,a4
50000374:	8d55                	or	a0,a0,a3
50000376:	00b31333          	sll	t1,t1,a1
5000037a:	00e8d733          	srl	a4,a7,a4
5000037e:	00676733          	or	a4,a4,t1
50000382:	01055313          	srli	t1,a0,0x10
50000386:	0267deb3          	divu	t4,a5,t1
5000038a:	01051813          	slli	a6,a0,0x10
5000038e:	01085813          	srli	a6,a6,0x10
50000392:	01075693          	srli	a3,a4,0x10
50000396:	00b61633          	sll	a2,a2,a1
5000039a:	0267f7b3          	remu	a5,a5,t1
5000039e:	03d80e33          	mul	t3,a6,t4
500003a2:	07c2                	slli	a5,a5,0x10
500003a4:	8edd                	or	a3,a3,a5
500003a6:	87f6                	mv	a5,t4
500003a8:	01c6fc63          	bgeu	a3,t3,500003c0 <__udivdi3+0x2c0>
500003ac:	96aa                	add	a3,a3,a0
500003ae:	fffe8793          	addi	a5,t4,-1
500003b2:	00a6e763          	bltu	a3,a0,500003c0 <__udivdi3+0x2c0>
500003b6:	01c6f563          	bgeu	a3,t3,500003c0 <__udivdi3+0x2c0>
500003ba:	ffee8793          	addi	a5,t4,-2
500003be:	96aa                	add	a3,a3,a0
500003c0:	41c686b3          	sub	a3,a3,t3
500003c4:	0266de33          	divu	t3,a3,t1
500003c8:	0742                	slli	a4,a4,0x10
500003ca:	8341                	srli	a4,a4,0x10
500003cc:	0266f6b3          	remu	a3,a3,t1
500003d0:	03c80833          	mul	a6,a6,t3
500003d4:	06c2                	slli	a3,a3,0x10
500003d6:	8f55                	or	a4,a4,a3
500003d8:	86f2                	mv	a3,t3
500003da:	01077c63          	bgeu	a4,a6,500003f2 <__udivdi3+0x2f2>
500003de:	972a                	add	a4,a4,a0
500003e0:	fffe0693          	addi	a3,t3,-1
500003e4:	00a76763          	bltu	a4,a0,500003f2 <__udivdi3+0x2f2>
500003e8:	01077563          	bgeu	a4,a6,500003f2 <__udivdi3+0x2f2>
500003ec:	ffee0693          	addi	a3,t3,-2
500003f0:	972a                	add	a4,a4,a0
500003f2:	07c2                	slli	a5,a5,0x10
500003f4:	00d7e533          	or	a0,a5,a3
500003f8:	01061313          	slli	t1,a2,0x10
500003fc:	06c2                	slli	a3,a3,0x10
500003fe:	82c1                	srli	a3,a3,0x10
50000400:	01035313          	srli	t1,t1,0x10
50000404:	8241                	srli	a2,a2,0x10
50000406:	41070733          	sub	a4,a4,a6
5000040a:	01055813          	srli	a6,a0,0x10
5000040e:	02668e33          	mul	t3,a3,t1
50000412:	02680333          	mul	t1,a6,t1
50000416:	010e5793          	srli	a5,t3,0x10
5000041a:	02c686b3          	mul	a3,a3,a2
5000041e:	969a                	add	a3,a3,t1
50000420:	97b6                	add	a5,a5,a3
50000422:	02c80833          	mul	a6,a6,a2
50000426:	0067f463          	bgeu	a5,t1,5000042e <__udivdi3+0x32e>
5000042a:	66c1                	lui	a3,0x10
5000042c:	9836                	add	a6,a6,a3
5000042e:	0107d693          	srli	a3,a5,0x10
50000432:	96c2                	add	a3,a3,a6
50000434:	00d76e63          	bltu	a4,a3,50000450 <__udivdi3+0x350>
50000438:	d8d717e3          	bne	a4,a3,500001c6 <__udivdi3+0xc6>
5000043c:	0e42                	slli	t3,t3,0x10
5000043e:	07c2                	slli	a5,a5,0x10
50000440:	010e5e13          	srli	t3,t3,0x10
50000444:	00b898b3          	sll	a7,a7,a1
50000448:	97f2                	add	a5,a5,t3
5000044a:	4581                	li	a1,0
5000044c:	d6f8fee3          	bgeu	a7,a5,500001c8 <__udivdi3+0xc8>
50000450:	157d                	addi	a0,a0,-1 # ffffff <RvExcFrameSize+0xffff6b>
50000452:	bb95                	j	500001c6 <__udivdi3+0xc6>
50000454:	4581                	li	a1,0
50000456:	4501                	li	a0,0
50000458:	8082                	ret

5000045a <ulp_lp_core_lp_timer_intr_handler>:
5000045a:	1141                	addi	sp,sp,-16
5000045c:	c606                	sw	ra,12(sp)
5000045e:	2529                	jal	50000a68 <ulp_lp_core_lp_timer_intr_clear>
50000460:	40b2                	lw	ra,12(sp)
50000462:	0141                	addi	sp,sp,16
50000464:	a901                	j	50000874 <ulp_lp_core_lp_timer_disable>

50000466 <sleep_store2>:
50000466:	1141                	addi	sp,sp,-16
50000468:	c606                	sw	ra,12(sp)
5000046a:	2375                	jal	50000a16 <ulp_lp_core_halt>

5000046c <wakeup>:
5000046c:	1141                	addi	sp,sp,-16
5000046e:	c606                	sw	ra,12(sp)
50000470:	2111                	jal	50000874 <ulp_lp_core_lp_timer_disable>
50000472:	2bbd                	jal	500009f0 <ulp_lp_core_wakeup_main_processor>
50000474:	234d                	jal	50000a16 <ulp_lp_core_halt>

50000476 <delayUs>:
50000476:	03100793          	li	a5,49
5000047a:	00a7c363          	blt	a5,a0,50000480 <delayUs+0xa>
5000047e:	a359                	j	50000a04 <ulp_lp_core_delay_us>
50000480:	1141                	addi	sp,sp,-16
50000482:	41f55593          	srai	a1,a0,0x1f
50000486:	c422                	sw	s0,8(sp)
50000488:	c606                	sw	ra,12(sp)
5000048a:	842a                	mv	s0,a0
5000048c:	2ebd                	jal	5000080a <ulp_lp_core_lp_timer_set_wakeup_time>
5000048e:	3e700793          	li	a5,999
50000492:	0287c263          	blt	a5,s0,500004b6 <delayUs+0x40>
50000496:	300467f3          	csrrsi	a5,mstatus,8
5000049a:	400007b7          	lui	a5,0x40000
5000049e:	3047a773          	csrrs	a4,mie,a5
500004a2:	10500073          	wfi
500004a6:	30047773          	csrrci	a4,mstatus,8
500004aa:	3047b7f3          	csrrc	a5,mie,a5
500004ae:	40b2                	lw	ra,12(sp)
500004b0:	4422                	lw	s0,8(sp)
500004b2:	0141                	addi	sp,sp,16
500004b4:	8082                	ret
500004b6:	4422                	lw	s0,8(sp)
500004b8:	600b07b7          	lui	a5,0x600b0
500004bc:	40b2                	lw	ra,12(sp)
500004be:	00078793          	mv	a5,a5
500004c2:	4741                	li	a4,16
500004c4:	18e79023          	sh	a4,384(a5) # 600b0180 <PMU+0x180>
500004c8:	0141                	addi	sp,sp,16
500004ca:	bec9                	j	5000009c <sleep_store>

500004cc <pulseIn>:
500004cc:	1141                	addi	sp,sp,-16
500004ce:	4681                	li	a3,0
500004d0:	00b6c8b3          	xor	a7,a3,a1
500004d4:	b0002873          	csrr	a6,mcycle
500004d8:	e20d                	bnez	a2,500004fa <pulseIn+0x2e>
500004da:	600b2737          	lui	a4,0x600b2
500004de:	00070793          	mv	a5,a4
500004e2:	53dc                	lw	a5,36(a5)
500004e4:	c43e                	sw	a5,8(sp)
500004e6:	00814783          	lbu	a5,8(sp)
500004ea:	40a7d7b3          	sra	a5,a5,a0
500004ee:	8b85                	andi	a5,a5,1
500004f0:	fef897e3          	bne	a7,a5,500004de <pulseIn+0x12>
500004f4:	ee85                	bnez	a3,5000052c <pulseIn+0x60>
500004f6:	4685                	li	a3,1
500004f8:	bfe1                	j	500004d0 <pulseIn+0x4>
500004fa:	00461793          	slli	a5,a2,0x4
500004fe:	17b1                	addi	a5,a5,-20
50000500:	97c2                	add	a5,a5,a6
50000502:	600b2337          	lui	t1,0x600b2
50000506:	b0002773          	csrr	a4,mcycle
5000050a:	00f76563          	bltu	a4,a5,50000514 <pulseIn+0x48>
5000050e:	4501                	li	a0,0
50000510:	0141                	addi	sp,sp,16
50000512:	8082                	ret
50000514:	00030713          	mv	a4,t1
50000518:	5358                	lw	a4,36(a4)
5000051a:	c63a                	sw	a4,12(sp)
5000051c:	00c14703          	lbu	a4,12(sp)
50000520:	40a75733          	sra	a4,a4,a0
50000524:	8b05                	andi	a4,a4,1
50000526:	fee890e3          	bne	a7,a4,50000506 <pulseIn+0x3a>
5000052a:	b7e9                	j	500004f4 <pulseIn+0x28>
5000052c:	b0002573          	csrr	a0,mcycle
50000530:	41050533          	sub	a0,a0,a6
50000534:	8111                	srli	a0,a0,0x4
50000536:	bfe9                	j	50000510 <pulseIn+0x44>

50000538 <sort>:
50000538:	157d                	addi	a0,a0,-1
5000053a:	87ae                	mv	a5,a1
5000053c:	4701                	li	a4,0
5000053e:	4681                	li	a3,0
50000540:	a821                	j	50000558 <sort+0x20>
50000542:	4390                	lw	a2,0(a5)
50000544:	0047a803          	lw	a6,4(a5)
50000548:	00c85663          	bge	a6,a2,50000554 <sort+0x1c>
5000054c:	0107a023          	sw	a6,0(a5)
50000550:	c3d0                	sw	a2,4(a5)
50000552:	4685                	li	a3,1
50000554:	0705                	addi	a4,a4,1 # 600b2001 <LP_IO+0x1>
50000556:	0791                	addi	a5,a5,4
50000558:	fea745e3          	blt	a4,a0,50000542 <sort+0xa>
5000055c:	fef9                	bnez	a3,5000053a <sort+0x2>
5000055e:	8082                	ret

50000560 <app_main>:
50000560:	711d                	addi	sp,sp,-96
50000562:	c8ca                	sw	s2,80(sp)
50000564:	c4d2                	sw	s4,72(sp)
50000566:	50001937          	lui	s2,0x50001
5000056a:	7a65                	lui	s4,0xffff9
5000056c:	c2d6                	sw	s5,68(sp)
5000056e:	c0da                	sw	s6,64(sp)
50000570:	de5e                	sw	s7,60(sp)
50000572:	dc62                	sw	s8,56(sp)
50000574:	da66                	sw	s9,52(sp)
50000576:	ce86                	sw	ra,92(sp)
50000578:	cca2                	sw	s0,88(sp)
5000057a:	caa6                	sw	s1,84(sp)
5000057c:	c6ce                	sw	s3,76(sp)
5000057e:	d86a                	sw	s10,48(sp)
50000580:	d66e                	sw	s11,44(sp)
50000582:	bec90913          	addi	s2,s2,-1044 # 50000bec <result>
50000586:	50001b37          	lui	s6,0x50001
5000058a:	600b3ab7          	lui	s5,0x600b3
5000058e:	08000bb7          	lui	s7,0x8000
50000592:	600b1c37          	lui	s8,0x600b1
50000596:	600b2cb7          	lui	s9,0x600b2
5000059a:	1a7d                	addi	s4,s4,-1 # ffff8fff <LPPERI+0x9ff467ff>
5000059c:	c64b0993          	addi	s3,s6,-924 # 50000c64 <buf>
500005a0:	c64b0d13          	addi	s10,s6,-924
500005a4:	04000d93          	li	s11,64
500005a8:	800a8693          	addi	a3,s5,-2048 # 600b2800 <LPPERI>
500005ac:	429c                	lw	a5,0(a3)
500005ae:	0177e7b3          	or	a5,a5,s7
500005b2:	c29c                	sw	a5,0(a3)
500005b4:	429c                	lw	a5,0(a3)
500005b6:	00479613          	slli	a2,a5,0x4
500005ba:	fe065de3          	bgez	a2,500005b4 <app_main+0x54>
500005be:	000c0793          	mv	a5,s8
500005c2:	5794                	lw	a3,40(a5)
500005c4:	000c8493          	mv	s1,s9
500005c8:	4529                	li	a0,10
500005ca:	cc36                	sw	a3,24(sp)
500005cc:	01814683          	lbu	a3,24(sp)
500005d0:	5790                	lw	a2,40(a5)
500005d2:	0d11                	addi	s10,s10,4
500005d4:	0406e693          	ori	a3,a3,64
500005d8:	ce32                	sw	a2,28(sp)
500005da:	00d10e23          	sb	a3,28(sp)
500005de:	46f2                	lw	a3,28(sp)
500005e0:	d794                	sw	a3,40(a5)
500005e2:	50bc                	lw	a5,96(s1)
500005e4:	0147f7b3          	and	a5,a5,s4
500005e8:	d0bc                	sw	a5,96(s1)
500005ea:	489c                	lw	a5,16(s1)
500005ec:	ca3e                	sw	a5,20(sp)
500005ee:	01b10a23          	sb	s11,20(sp)
500005f2:	47d2                	lw	a5,20(sp)
500005f4:	c89c                	sw	a5,16(s1)
500005f6:	40dc                	lw	a5,4(s1)
500005f8:	c83e                	sw	a5,16(sp)
500005fa:	01b10823          	sb	s11,16(sp)
500005fe:	47c2                	lw	a5,16(sp)
50000600:	c0dc                	sw	a5,4(s1)
50000602:	2109                	jal	50000a04 <ulp_lp_core_delay_us>
50000604:	449c                	lw	a5,8(s1)
50000606:	00031637          	lui	a2,0x31
5000060a:	d4060613          	addi	a2,a2,-704 # 30d40 <RvExcFrameSize+0x30cac>
5000060e:	c63e                	sw	a5,12(sp)
50000610:	01b10623          	sb	s11,12(sp)
50000614:	47b2                	lw	a5,12(sp)
50000616:	4585                	li	a1,1
50000618:	4501                	li	a0,0
5000061a:	c49c                	sw	a5,8(s1)
5000061c:	3d45                	jal	500004cc <pulseIn>
5000061e:	fead2e23          	sw	a0,-4(s10)
50000622:	00049537          	lui	a0,0x49
50000626:	1561                	addi	a0,a0,-8 # 48ff8 <RvExcFrameSize+0x48f64>
50000628:	35b9                	jal	50000476 <delayUs>
5000062a:	500017b7          	lui	a5,0x50001
5000062e:	cb478813          	addi	a6,a5,-844 # 50000cb4 <mrbc_sp_bottom>
50000632:	f7a81be3          	bne	a6,s10,500005a8 <app_main+0x48>
50000636:	c64b0593          	addi	a1,s6,-924
5000063a:	4551                	li	a0,20
5000063c:	3df5                	jal	50000538 <sort>
5000063e:	04098693          	addi	a3,s3,64
50000642:	4781                	li	a5,0
50000644:	0089a603          	lw	a2,8(s3)
50000648:	0991                	addi	s3,s3,4
5000064a:	97b2                	add	a5,a5,a2
5000064c:	ff369ce3          	bne	a3,s3,50000644 <app_main+0xe4>
50000650:	46c1                	li	a3,16
50000652:	02d7c7b3          	div	a5,a5,a3
50000656:	0911                	addi	s2,s2,4
50000658:	fef92e23          	sw	a5,-4(s2)
5000065c:	500017b7          	lui	a5,0x50001
50000660:	bf478793          	addi	a5,a5,-1036 # 50000bf4 <result+0x8>
50000664:	f3279ce3          	bne	a5,s2,5000059c <app_main+0x3c>
50000668:	40f6                	lw	ra,92(sp)
5000066a:	4466                	lw	s0,88(sp)
5000066c:	44d6                	lw	s1,84(sp)
5000066e:	4946                	lw	s2,80(sp)
50000670:	49b6                	lw	s3,76(sp)
50000672:	4a26                	lw	s4,72(sp)
50000674:	4a96                	lw	s5,68(sp)
50000676:	4b06                	lw	s6,64(sp)
50000678:	5bf2                	lw	s7,60(sp)
5000067a:	5c62                	lw	s8,56(sp)
5000067c:	5cd2                	lw	s9,52(sp)
5000067e:	5d42                	lw	s10,48(sp)
50000680:	5db2                	lw	s11,44(sp)
50000682:	4501                	li	a0,0
50000684:	6125                	addi	sp,sp,96
50000686:	8082                	ret

50000688 <main>:
50000688:	1141                	addi	sp,sp,-16
5000068a:	c606                	sw	ra,12(sp)
5000068c:	c422                	sw	s0,8(sp)
5000068e:	7139                	addi	sp,sp,-64
50000690:	50001437          	lui	s0,0x50001
50000694:	cb442783          	lw	a5,-844(s0) # 50000cb4 <mrbc_sp_bottom>
50000698:	c391                	beqz	a5,5000069c <main+0x14>
5000069a:	3c15                	jal	500000ce <sleep_restore>
5000069c:	4505                	li	a0,1
5000069e:	2e55                	jal	50000a52 <ulp_lp_core_lp_timer_intr_enable>
500006a0:	35c1                	jal	50000560 <app_main>
500006a2:	ca042a23          	sw	zero,-844(s0)
500006a6:	33d9                	jal	5000046c <wakeup>

500006a8 <_panic_handler>:
500006a8:	7135                	addi	sp,sp,-160
500006aa:	c206                	sw	ra,4(sp)
500006ac:	c812                	sw	tp,16(sp)
500006ae:	ca16                	sw	t0,20(sp)
500006b0:	cc1a                	sw	t1,24(sp)
500006b2:	ce1e                	sw	t2,28(sp)
500006b4:	d022                	sw	s0,32(sp)
500006b6:	d226                	sw	s1,36(sp)
500006b8:	d42a                	sw	a0,40(sp)
500006ba:	d62e                	sw	a1,44(sp)
500006bc:	d832                	sw	a2,48(sp)
500006be:	da36                	sw	a3,52(sp)
500006c0:	dc3a                	sw	a4,56(sp)
500006c2:	de3e                	sw	a5,60(sp)
500006c4:	c0c2                	sw	a6,64(sp)
500006c6:	c2c6                	sw	a7,68(sp)
500006c8:	c4ca                	sw	s2,72(sp)
500006ca:	c6ce                	sw	s3,76(sp)
500006cc:	c8d2                	sw	s4,80(sp)
500006ce:	cad6                	sw	s5,84(sp)
500006d0:	ccda                	sw	s6,88(sp)
500006d2:	cede                	sw	s7,92(sp)
500006d4:	d0e2                	sw	s8,96(sp)
500006d6:	d2e6                	sw	s9,100(sp)
500006d8:	d4ea                	sw	s10,104(sp)
500006da:	d6ee                	sw	s11,108(sp)
500006dc:	d8f2                	sw	t3,112(sp)
500006de:	daf6                	sw	t4,116(sp)
500006e0:	dcfa                	sw	t5,120(sp)
500006e2:	defe                	sw	t6,124(sp)
500006e4:	341022f3          	csrr	t0,mepc
500006e8:	c016                	sw	t0,0(sp)
500006ea:	0a010293          	addi	t0,sp,160
500006ee:	c416                	sw	t0,8(sp)
500006f0:	300022f3          	csrr	t0,mstatus
500006f4:	c116                	sw	t0,128(sp)
500006f6:	342022f3          	csrr	t0,mcause
500006fa:	c516                	sw	t0,136(sp)
500006fc:	305022f3          	csrr	t0,mtvec
50000700:	c316                	sw	t0,132(sp)
50000702:	f14022f3          	csrr	t0,mhartid
50000706:	c916                	sw	t0,144(sp)
50000708:	343022f3          	csrr	t0,mtval
5000070c:	c716                	sw	t0,140(sp)
5000070e:	342025f3          	csrr	a1,mcause
50000712:	850a                	mv	a0,sp
50000714:	26bd                	jal	50000a82 <ulp_lp_core_panic_handler>

50000716 <_end>:
50000716:	a001                	j	50000716 <_end>

50000718 <_interrupt_handler>:
50000718:	7119                	addi	sp,sp,-128
5000071a:	c206                	sw	ra,4(sp)
5000071c:	c812                	sw	tp,16(sp)
5000071e:	ca16                	sw	t0,20(sp)
50000720:	cc1a                	sw	t1,24(sp)
50000722:	ce1e                	sw	t2,28(sp)
50000724:	d022                	sw	s0,32(sp)
50000726:	d226                	sw	s1,36(sp)
50000728:	d42a                	sw	a0,40(sp)
5000072a:	d62e                	sw	a1,44(sp)
5000072c:	d832                	sw	a2,48(sp)
5000072e:	da36                	sw	a3,52(sp)
50000730:	dc3a                	sw	a4,56(sp)
50000732:	de3e                	sw	a5,60(sp)
50000734:	c0c2                	sw	a6,64(sp)
50000736:	c2c6                	sw	a7,68(sp)
50000738:	c4ca                	sw	s2,72(sp)
5000073a:	c6ce                	sw	s3,76(sp)
5000073c:	c8d2                	sw	s4,80(sp)
5000073e:	cad6                	sw	s5,84(sp)
50000740:	ccda                	sw	s6,88(sp)
50000742:	cede                	sw	s7,92(sp)
50000744:	d0e2                	sw	s8,96(sp)
50000746:	d2e6                	sw	s9,100(sp)
50000748:	d4ea                	sw	s10,104(sp)
5000074a:	d6ee                	sw	s11,108(sp)
5000074c:	d8f2                	sw	t3,112(sp)
5000074e:	daf6                	sw	t4,116(sp)
50000750:	dcfa                	sw	t5,120(sp)
50000752:	defe                	sw	t6,124(sp)
50000754:	341022f3          	csrr	t0,mepc
50000758:	c016                	sw	t0,0(sp)
5000075a:	263d                	jal	50000a88 <ulp_lp_core_intr_handler>
5000075c:	4282                	lw	t0,0(sp)
5000075e:	34129073          	csrw	mepc,t0
50000762:	4092                	lw	ra,4(sp)
50000764:	4242                	lw	tp,16(sp)
50000766:	42d2                	lw	t0,20(sp)
50000768:	4362                	lw	t1,24(sp)
5000076a:	43f2                	lw	t2,28(sp)
5000076c:	5402                	lw	s0,32(sp)
5000076e:	5492                	lw	s1,36(sp)
50000770:	5522                	lw	a0,40(sp)
50000772:	55b2                	lw	a1,44(sp)
50000774:	5642                	lw	a2,48(sp)
50000776:	56d2                	lw	a3,52(sp)
50000778:	5762                	lw	a4,56(sp)
5000077a:	57f2                	lw	a5,60(sp)
5000077c:	4806                	lw	a6,64(sp)
5000077e:	4896                	lw	a7,68(sp)
50000780:	4926                	lw	s2,72(sp)
50000782:	49b6                	lw	s3,76(sp)
50000784:	4a46                	lw	s4,80(sp)
50000786:	4ad6                	lw	s5,84(sp)
50000788:	4b66                	lw	s6,88(sp)
5000078a:	4bf6                	lw	s7,92(sp)
5000078c:	5c06                	lw	s8,96(sp)
5000078e:	5c96                	lw	s9,100(sp)
50000790:	5d26                	lw	s10,104(sp)
50000792:	5db6                	lw	s11,108(sp)
50000794:	5e46                	lw	t3,112(sp)
50000796:	5ed6                	lw	t4,116(sp)
50000798:	5f66                	lw	t5,120(sp)
5000079a:	5ff6                	lw	t6,124(sp)
5000079c:	6109                	addi	sp,sp,128
5000079e:	30200073          	mret

500007a2 <ulp_lp_core_memory_shared_cfg_get>:
500007a2:	50001537          	lui	a0,0x50001
500007a6:	ff050513          	addi	a0,a0,-16 # 50000ff0 <s_shared_mem>
500007aa:	8082                	ret

500007ac <lp_timer_hal_set_alarm_target>:
500007ac:	600b17b7          	lui	a5,0x600b1
500007b0:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
500007b4:	43f8                	lw	a4,68(a5)
500007b6:	800006b7          	lui	a3,0x80000
500007ba:	1141                	addi	sp,sp,-16
500007bc:	8f55                	or	a4,a4,a3
500007be:	c3f8                	sw	a4,68(a5)
500007c0:	47d0                	lw	a2,12(a5)
500007c2:	05c2                	slli	a1,a1,0x10
500007c4:	81c1                	srli	a1,a1,0x10
500007c6:	c432                	sw	a2,8(sp)
500007c8:	00b11423          	sh	a1,8(sp)
500007cc:	4622                	lw	a2,8(sp)
500007ce:	c7d0                	sw	a2,12(a5)
500007d0:	4790                	lw	a2,8(a5)
500007d2:	c632                	sw	a2,12(sp)
500007d4:	c62a                	sw	a0,12(sp)
500007d6:	4732                	lw	a4,12(sp)
500007d8:	c798                	sw	a4,8(a5)
500007da:	47d8                	lw	a4,12(a5)
500007dc:	8f55                	or	a4,a4,a3
500007de:	c7d8                	sw	a4,12(a5)
500007e0:	0141                	addi	sp,sp,16
500007e2:	8082                	ret

500007e4 <ulp_lp_core_lp_timer_get_cycle_count>:
500007e4:	600b17b7          	lui	a5,0x600b1
500007e8:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
500007ec:	4b98                	lw	a4,16(a5)
500007ee:	100006b7          	lui	a3,0x10000
500007f2:	1101                	addi	sp,sp,-32
500007f4:	8f55                	or	a4,a4,a3
500007f6:	cb98                	sw	a4,16(a5)
500007f8:	4bd8                	lw	a4,20(a5)
500007fa:	ce3a                	sw	a4,28(sp)
500007fc:	4572                	lw	a0,28(sp)
500007fe:	4f9c                	lw	a5,24(a5)
50000800:	cc3e                	sw	a5,24(sp)
50000802:	01815583          	lhu	a1,24(sp)
50000806:	6105                	addi	sp,sp,32
50000808:	8082                	ret

5000080a <ulp_lp_core_lp_timer_set_wakeup_time>:
5000080a:	1101                	addi	sp,sp,-32
5000080c:	ce06                	sw	ra,28(sp)
5000080e:	cc22                	sw	s0,24(sp)
50000810:	ca26                	sw	s1,20(sp)
50000812:	c84a                	sw	s2,16(sp)
50000814:	c64e                	sw	s3,12(sp)
50000816:	892a                	mv	s2,a0
50000818:	84ae                	mv	s1,a1
5000081a:	37e9                	jal	500007e4 <ulp_lp_core_lp_timer_get_cycle_count>
5000081c:	600b17b7          	lui	a5,0x600b1
50000820:	43d0                	lw	a2,4(a5)
50000822:	89ae                	mv	s3,a1
50000824:	04ce                	slli	s1,s1,0x13
50000826:	00d95593          	srli	a1,s2,0xd
5000082a:	842a                	mv	s0,a0
5000082c:	8dc5                	or	a1,a1,s1
5000082e:	01391513          	slli	a0,s2,0x13
50000832:	4681                	li	a3,0
50000834:	30f1                	jal	50000100 <__udivdi3>
50000836:	87aa                	mv	a5,a0
50000838:	9522                	add	a0,a0,s0
5000083a:	4462                	lw	s0,24(sp)
5000083c:	40f2                	lw	ra,28(sp)
5000083e:	44d2                	lw	s1,20(sp)
50000840:	4942                	lw	s2,16(sp)
50000842:	95ce                	add	a1,a1,s3
50000844:	49b2                	lw	s3,12(sp)
50000846:	00f537b3          	sltu	a5,a0,a5
5000084a:	95be                	add	a1,a1,a5
5000084c:	6105                	addi	sp,sp,32
5000084e:	bfb9                	j	500007ac <lp_timer_hal_set_alarm_target>

50000850 <ulp_lp_core_lp_timer_set_wakeup_ticks>:
50000850:	1141                	addi	sp,sp,-16
50000852:	c422                	sw	s0,8(sp)
50000854:	c226                	sw	s1,4(sp)
50000856:	c606                	sw	ra,12(sp)
50000858:	842a                	mv	s0,a0
5000085a:	84ae                	mv	s1,a1
5000085c:	3761                	jal	500007e4 <ulp_lp_core_lp_timer_get_cycle_count>
5000085e:	87aa                	mv	a5,a0
50000860:	9522                	add	a0,a0,s0
50000862:	4422                	lw	s0,8(sp)
50000864:	40b2                	lw	ra,12(sp)
50000866:	95a6                	add	a1,a1,s1
50000868:	4492                	lw	s1,4(sp)
5000086a:	00f537b3          	sltu	a5,a0,a5
5000086e:	95be                	add	a1,a1,a5
50000870:	0141                	addi	sp,sp,16
50000872:	bf2d                	j	500007ac <lp_timer_hal_set_alarm_target>

50000874 <ulp_lp_core_lp_timer_disable>:
50000874:	600b17b7          	lui	a5,0x600b1
50000878:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
5000087c:	47d8                	lw	a4,12(a5)
5000087e:	800006b7          	lui	a3,0x80000
50000882:	fff68613          	addi	a2,a3,-1 # 7fffffff <LPPERI+0x1ff4d7ff>
50000886:	8f71                	and	a4,a4,a2
50000888:	c7d8                	sw	a4,12(a5)
5000088a:	43f8                	lw	a4,68(a5)
5000088c:	8f55                	or	a4,a4,a3
5000088e:	c3f8                	sw	a4,68(a5)
50000890:	8082                	ret

50000892 <lp_core_startup>:
50000892:	1141                	addi	sp,sp,-16
50000894:	c606                	sw	ra,12(sp)
50000896:	2819                	jal	500008ac <ulp_lp_core_update_wakeup_cause>
50000898:	3bc5                	jal	50000688 <main>
5000089a:	3721                	jal	500007a2 <ulp_lp_core_memory_shared_cfg_get>
5000089c:	87aa                	mv	a5,a0
5000089e:	47cc                	lw	a1,12(a5)
500008a0:	4508                	lw	a0,8(a0)
500008a2:	00b567b3          	or	a5,a0,a1
500008a6:	c391                	beqz	a5,500008aa <lp_core_startup+0x18>
500008a8:	3765                	jal	50000850 <ulp_lp_core_lp_timer_set_wakeup_ticks>
500008aa:	22b5                	jal	50000a16 <ulp_lp_core_halt>

500008ac <ulp_lp_core_update_wakeup_cause>:
500008ac:	600b0737          	lui	a4,0x600b0
500008b0:	00070693          	mv	a3,a4
500008b4:	1806d683          	lhu	a3,384(a3)
500008b8:	500017b7          	lui	a5,0x50001
500008bc:	ca07ac23          	sw	zero,-840(a5) # 50000cb8 <lp_wakeup_cause>
500008c0:	8a85                	andi	a3,a3,1
500008c2:	00070713          	mv	a4,a4
500008c6:	ce81                	beqz	a3,500008de <ulp_lp_core_update_wakeup_cause+0x32>
500008c8:	16c72683          	lw	a3,364(a4) # 600b016c <PMU+0x16c>
500008cc:	0006d963          	bgez	a3,500008de <ulp_lp_core_update_wakeup_cause+0x32>
500008d0:	4685                	li	a3,1
500008d2:	cad7ac23          	sw	a3,-840(a5)
500008d6:	800006b7          	lui	a3,0x80000
500008da:	16d72c23          	sw	a3,376(a4)
500008de:	18075683          	lhu	a3,384(a4)
500008e2:	8a89                	andi	a3,a3,2
500008e4:	c29d                	beqz	a3,5000090a <ulp_lp_core_update_wakeup_cause+0x5e>
500008e6:	600b16b7          	lui	a3,0x600b1
500008ea:	40068693          	addi	a3,a3,1024 # 600b1400 <LP_UART>
500008ee:	42d0                	lw	a2,4(a3)
500008f0:	00c61593          	slli	a1,a2,0xc
500008f4:	0005db63          	bgez	a1,5000090a <ulp_lp_core_update_wakeup_cause+0x5e>
500008f8:	cb87a603          	lw	a2,-840(a5)
500008fc:	00266613          	ori	a2,a2,2
50000900:	cac7ac23          	sw	a2,-840(a5)
50000904:	00080637          	lui	a2,0x80
50000908:	ca90                	sw	a2,16(a3)
5000090a:	18075683          	lhu	a3,384(a4)
5000090e:	8a91                	andi	a3,a3,4
50000910:	c6c1                	beqz	a3,50000998 <ulp_lp_core_update_wakeup_cause+0xec>
50000912:	600b26b7          	lui	a3,0x600b2
50000916:	00068693          	mv	a3,a3
5000091a:	4e90                	lw	a2,24(a3)
5000091c:	1141                	addi	sp,sp,-16
5000091e:	c432                	sw	a2,8(sp)
50000920:	00814603          	lbu	a2,8(sp)
50000924:	ce11                	beqz	a2,50000940 <ulp_lp_core_update_wakeup_cause+0x94>
50000926:	cb87a603          	lw	a2,-840(a5)
5000092a:	00466613          	ori	a2,a2,4
5000092e:	cac7ac23          	sw	a2,-840(a5)
50000932:	5290                	lw	a2,32(a3)
50000934:	c632                	sw	a2,12(sp)
50000936:	567d                	li	a2,-1
50000938:	00c10623          	sb	a2,12(sp)
5000093c:	4632                	lw	a2,12(sp)
5000093e:	d290                	sw	a2,32(a3)
50000940:	18075683          	lhu	a3,384(a4)
50000944:	8aa1                	andi	a3,a3,8
50000946:	c29d                	beqz	a3,5000096c <ulp_lp_core_update_wakeup_cause+0xc0>
50000948:	600b16b7          	lui	a3,0x600b1
5000094c:	00068693          	mv	a3,a3
50000950:	4ab0                	lw	a2,80(a3)
50000952:	8205                	srli	a2,a2,0x1
50000954:	8a05                	andi	a2,a2,1
50000956:	ca19                	beqz	a2,5000096c <ulp_lp_core_update_wakeup_cause+0xc0>
50000958:	cb87a603          	lw	a2,-840(a5)
5000095c:	00866613          	ori	a2,a2,8
50000960:	cac7ac23          	sw	a2,-840(a5)
50000964:	4ab0                	lw	a2,80(a3)
50000966:	00166613          	ori	a2,a2,1
5000096a:	cab0                	sw	a2,80(a3)
5000096c:	18075703          	lhu	a4,384(a4)
50000970:	8b41                	andi	a4,a4,16
50000972:	c30d                	beqz	a4,50000994 <ulp_lp_core_update_wakeup_cause+0xe8>
50000974:	600b1737          	lui	a4,0x600b1
50000978:	c0070713          	addi	a4,a4,-1024 # 600b0c00 <LP_TIMER>
5000097c:	5f14                	lw	a3,56(a4)
5000097e:	0006db63          	bgez	a3,50000994 <ulp_lp_core_update_wakeup_cause+0xe8>
50000982:	cb87a683          	lw	a3,-840(a5)
50000986:	0106e693          	ori	a3,a3,16
5000098a:	cad7ac23          	sw	a3,-840(a5)
5000098e:	800007b7          	lui	a5,0x80000
50000992:	c37c                	sw	a5,68(a4)
50000994:	0141                	addi	sp,sp,16
50000996:	8082                	ret
50000998:	18075683          	lhu	a3,384(a4)
5000099c:	8aa1                	andi	a3,a3,8
5000099e:	c29d                	beqz	a3,500009c4 <ulp_lp_core_update_wakeup_cause+0x118>
500009a0:	600b16b7          	lui	a3,0x600b1
500009a4:	00068693          	mv	a3,a3
500009a8:	4ab0                	lw	a2,80(a3)
500009aa:	8205                	srli	a2,a2,0x1
500009ac:	8a05                	andi	a2,a2,1
500009ae:	ca19                	beqz	a2,500009c4 <ulp_lp_core_update_wakeup_cause+0x118>
500009b0:	cb87a603          	lw	a2,-840(a5) # 7ffffcb8 <LPPERI+0x1ff4d4b8>
500009b4:	00866613          	ori	a2,a2,8
500009b8:	cac7ac23          	sw	a2,-840(a5)
500009bc:	4ab0                	lw	a2,80(a3)
500009be:	00166613          	ori	a2,a2,1
500009c2:	cab0                	sw	a2,80(a3)
500009c4:	18075703          	lhu	a4,384(a4)
500009c8:	8b41                	andi	a4,a4,16
500009ca:	c315                	beqz	a4,500009ee <ulp_lp_core_update_wakeup_cause+0x142>
500009cc:	600b1737          	lui	a4,0x600b1
500009d0:	c0070713          	addi	a4,a4,-1024 # 600b0c00 <LP_TIMER>
500009d4:	5f14                	lw	a3,56(a4)
500009d6:	0006dc63          	bgez	a3,500009ee <ulp_lp_core_update_wakeup_cause+0x142>
500009da:	cb87a683          	lw	a3,-840(a5)
500009de:	0106e693          	ori	a3,a3,16
500009e2:	cad7ac23          	sw	a3,-840(a5)
500009e6:	800007b7          	lui	a5,0x80000
500009ea:	c37c                	sw	a5,68(a4)
500009ec:	8082                	ret
500009ee:	8082                	ret

500009f0 <ulp_lp_core_wakeup_main_processor>:
500009f0:	600b07b7          	lui	a5,0x600b0
500009f4:	18478793          	addi	a5,a5,388 # 600b0184 <PMU+0x184>
500009f8:	4398                	lw	a4,0(a5)
500009fa:	400006b7          	lui	a3,0x40000
500009fe:	8f55                	or	a4,a4,a3
50000a00:	c398                	sw	a4,0(a5)
50000a02:	8082                	ret

50000a04 <ulp_lp_core_delay_us>:
50000a04:	b0002773          	csrr	a4,mcycle
50000a08:	0512                	slli	a0,a0,0x4
50000a0a:	b00027f3          	csrr	a5,mcycle
50000a0e:	8f99                	sub	a5,a5,a4
50000a10:	fea7ede3          	bltu	a5,a0,50000a0a <ulp_lp_core_delay_us+0x6>
50000a14:	8082                	ret

50000a16 <ulp_lp_core_halt>:
50000a16:	600b07b7          	lui	a5,0x600b0
50000a1a:	00078793          	mv	a5,a5
50000a1e:	1807a703          	lw	a4,384(a5) # 600b0180 <PMU+0x180>
50000a22:	800006b7          	lui	a3,0x80000
50000a26:	8f55                	or	a4,a4,a3
50000a28:	18e7a023          	sw	a4,384(a5)
50000a2c:	a001                	j	50000a2c <ulp_lp_core_halt+0x16>

50000a2e <ulp_lp_core_stop_lp_core>:
50000a2e:	600b07b7          	lui	a5,0x600b0
50000a32:	00078793          	mv	a5,a5
50000a36:	18079023          	sh	zero,384(a5) # 600b0180 <PMU+0x180>
50000a3a:	1807a703          	lw	a4,384(a5)
50000a3e:	800006b7          	lui	a3,0x80000
50000a42:	8f55                	or	a4,a4,a3
50000a44:	18e7a023          	sw	a4,384(a5)
50000a48:	8082                	ret

50000a4a <ulp_lp_core_abort>:
50000a4a:	1141                	addi	sp,sp,-16
50000a4c:	c606                	sw	ra,12(sp)
50000a4e:	37c5                	jal	50000a2e <ulp_lp_core_stop_lp_core>
50000a50:	a001                	j	50000a50 <ulp_lp_core_abort+0x6>

50000a52 <ulp_lp_core_lp_timer_intr_enable>:
50000a52:	600b1737          	lui	a4,0x600b1
50000a56:	c0070713          	addi	a4,a4,-1024 # 600b0c00 <LP_TIMER>
50000a5a:	433c                	lw	a5,64(a4)
50000a5c:	057e                	slli	a0,a0,0x1f
50000a5e:	0786                	slli	a5,a5,0x1
50000a60:	8385                	srli	a5,a5,0x1
50000a62:	8fc9                	or	a5,a5,a0
50000a64:	c33c                	sw	a5,64(a4)
50000a66:	8082                	ret

50000a68 <ulp_lp_core_lp_timer_intr_clear>:
50000a68:	600b17b7          	lui	a5,0x600b1
50000a6c:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
50000a70:	43f8                	lw	a4,68(a5)
50000a72:	800006b7          	lui	a3,0x80000
50000a76:	8f55                	or	a4,a4,a3
50000a78:	c3f8                	sw	a4,68(a5)
50000a7a:	8082                	ret

50000a7c <ulp_lp_core_efuse_intr_handler>:
50000a7c:	1141                	addi	sp,sp,-16
50000a7e:	c606                	sw	ra,12(sp)
50000a80:	37e9                	jal	50000a4a <ulp_lp_core_abort>

50000a82 <ulp_lp_core_panic_handler>:
50000a82:	1141                	addi	sp,sp,-16
50000a84:	c606                	sw	ra,12(sp)
50000a86:	37d1                	jal	50000a4a <ulp_lp_core_abort>

50000a88 <ulp_lp_core_intr_handler>:
50000a88:	600b37b7          	lui	a5,0x600b3
50000a8c:	1101                	addi	sp,sp,-32
50000a8e:	80078793          	addi	a5,a5,-2048 # 600b2800 <LPPERI>
50000a92:	c84a                	sw	s2,16(sp)
50000a94:	0207a903          	lw	s2,32(a5)
50000a98:	cc22                	sw	s0,24(sp)
50000a9a:	50001437          	lui	s0,0x50001
50000a9e:	ca26                	sw	s1,20(sp)
50000aa0:	c64e                	sw	s3,12(sp)
50000aa2:	ce06                	sw	ra,28(sp)
50000aa4:	03f97913          	andi	s2,s2,63
50000aa8:	bd440413          	addi	s0,s0,-1068 # 50000bd4 <s_intr_handlers>
50000aac:	4481                	li	s1,0
50000aae:	4999                	li	s3,6
50000ab0:	409957b3          	sra	a5,s2,s1
50000ab4:	8b85                	andi	a5,a5,1
50000ab6:	c781                	beqz	a5,50000abe <ulp_lp_core_intr_handler+0x36>
50000ab8:	401c                	lw	a5,0(s0)
50000aba:	c391                	beqz	a5,50000abe <ulp_lp_core_intr_handler+0x36>
50000abc:	9782                	jalr	a5
50000abe:	0485                	addi	s1,s1,1
50000ac0:	0411                	addi	s0,s0,4
50000ac2:	ff3497e3          	bne	s1,s3,50000ab0 <ulp_lp_core_intr_handler+0x28>
50000ac6:	40f2                	lw	ra,28(sp)
50000ac8:	4462                	lw	s0,24(sp)
50000aca:	44d2                	lw	s1,20(sp)
50000acc:	4942                	lw	s2,16(sp)
50000ace:	49b2                	lw	s3,12(sp)
50000ad0:	6105                	addi	sp,sp,32
50000ad2:	8082                	ret
