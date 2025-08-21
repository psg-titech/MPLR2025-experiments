
.\build\esp-idf\main\ulp_main\ulp_main.elf:     file format elf32-littleriscv


Disassembly of section .vector.text:

50000000 <_vector_table>:
50000000:	4b10006f          	j	50000cb0 <_panic_handler>
50000004:	4ad0006f          	j	50000cb0 <_panic_handler>
50000008:	4a90006f          	j	50000cb0 <_panic_handler>
5000000c:	4a50006f          	j	50000cb0 <_panic_handler>
50000010:	4a10006f          	j	50000cb0 <_panic_handler>
50000014:	49d0006f          	j	50000cb0 <_panic_handler>
50000018:	4990006f          	j	50000cb0 <_panic_handler>
5000001c:	4950006f          	j	50000cb0 <_panic_handler>
50000020:	4910006f          	j	50000cb0 <_panic_handler>
50000024:	48d0006f          	j	50000cb0 <_panic_handler>
50000028:	4890006f          	j	50000cb0 <_panic_handler>
5000002c:	4850006f          	j	50000cb0 <_panic_handler>
50000030:	4810006f          	j	50000cb0 <_panic_handler>
50000034:	47d0006f          	j	50000cb0 <_panic_handler>
50000038:	4790006f          	j	50000cb0 <_panic_handler>
5000003c:	4750006f          	j	50000cb0 <_panic_handler>
50000040:	4710006f          	j	50000cb0 <_panic_handler>
50000044:	46d0006f          	j	50000cb0 <_panic_handler>
50000048:	4690006f          	j	50000cb0 <_panic_handler>
5000004c:	4650006f          	j	50000cb0 <_panic_handler>
50000050:	4610006f          	j	50000cb0 <_panic_handler>
50000054:	45d0006f          	j	50000cb0 <_panic_handler>
50000058:	4590006f          	j	50000cb0 <_panic_handler>
5000005c:	4550006f          	j	50000cb0 <_panic_handler>
50000060:	4510006f          	j	50000cb0 <_panic_handler>
50000064:	44d0006f          	j	50000cb0 <_panic_handler>
50000068:	4490006f          	j	50000cb0 <_panic_handler>
5000006c:	4450006f          	j	50000cb0 <_panic_handler>
50000070:	4410006f          	j	50000cb0 <_panic_handler>
50000074:	43d0006f          	j	50000cb0 <_panic_handler>
50000078:	4a90006f          	j	50000d20 <_interrupt_handler>
5000007c:	4350006f          	j	50000cb0 <_panic_handler>

Disassembly of section .text:

50000080 <reset_vector>:
50000080:	00000297          	auipc	t0,0x0
50000084:	f8028293          	addi	t0,t0,-128 # 50000000 <_vector_table>
50000088:	30529073          	csrw	mtvec,t0
5000008c:	a009                	j	5000008e <__start>

5000008e <__start>:
5000008e:	00003117          	auipc	sp,0x3
50000092:	61210113          	addi	sp,sp,1554 # 500036a0 <s_shared_mem>
50000096:	607000ef          	jal	50000e9c <lp_core_startup>

5000009a <loop>:
5000009a:	a001                	j	5000009a <loop>

5000009c <fallback>:
5000009c:	7119                	addi	sp,sp,-128
5000009e:	c006                	sw	ra,0(sp)
500000a0:	c20e                	sw	gp,4(sp)
500000a2:	c812                	sw	tp,16(sp)
500000a4:	ca16                	sw	t0,20(sp)
500000a6:	cc1a                	sw	t1,24(sp)
500000a8:	ce1e                	sw	t2,28(sp)
500000aa:	d022                	sw	s0,32(sp)
500000ac:	d226                	sw	s1,36(sp)
500000ae:	d42a                	sw	a0,40(sp)
500000b0:	d62e                	sw	a1,44(sp)
500000b2:	d832                	sw	a2,48(sp)
500000b4:	da36                	sw	a3,52(sp)
500000b6:	dc3a                	sw	a4,56(sp)
500000b8:	de3e                	sw	a5,60(sp)
500000ba:	c0c2                	sw	a6,64(sp)
500000bc:	c2c6                	sw	a7,68(sp)
500000be:	c4ca                	sw	s2,72(sp)
500000c0:	c6ce                	sw	s3,76(sp)
500000c2:	c8d2                	sw	s4,80(sp)
500000c4:	cad6                	sw	s5,84(sp)
500000c6:	ccda                	sw	s6,88(sp)
500000c8:	cede                	sw	s7,92(sp)
500000ca:	d0e2                	sw	s8,96(sp)
500000cc:	d2e6                	sw	s9,100(sp)
500000ce:	d4ea                	sw	s10,104(sp)
500000d0:	d6ee                	sw	s11,108(sp)
500000d2:	d8f2                	sw	t3,112(sp)
500000d4:	daf6                	sw	t4,116(sp)
500000d6:	dcfa                	sw	t5,120(sp)
500000d8:	defe                	sw	t6,124(sp)
500000da:	23cd                	jal	500006bc <fallback_post>

500000dc <read_barrier>:
500000dc:	1101                	addi	sp,sp,-32
500000de:	c22a                	sw	a0,4(sp)
500000e0:	c42e                	sw	a1,8(sp)
500000e2:	852e                	mv	a0,a1
500000e4:	4108                	lw	a0,0(a0)
500000e6:	c131                	beqz	a0,5000012a <readbarrier_this_is_nil>
500000e8:	00157593          	andi	a1,a0,1
500000ec:	e5a1                	bnez	a1,50000134 <readbarrier_this_is_int>
500000ee:	40355593          	srai	a1,a0,0x3
500000f2:	cd95                	beqz	a1,5000012e <readbarrier_this_is_bool>
500000f4:	ce2a                	sw	a0,28(sp)
500000f6:	500005b7          	lui	a1,0x50000
500000fa:	8d0d                	sub	a0,a0,a1
500000fc:	6591                	lui	a1,0x4
500000fe:	02b56263          	bltu	a0,a1,50000122 <readbarrier_translation_skip>
50000102:	c006                	sw	ra,0(sp)
50000104:	c632                	sw	a2,12(sp)
50000106:	c836                	sw	a3,16(sp)
50000108:	ca3a                	sw	a4,20(sp)
5000010a:	cc3e                	sw	a5,24(sp)
5000010c:	4572                	lw	a0,28(sp)
5000010e:	712000ef          	jal	50000820 <readfail>
50000112:	4632                	lw	a2,12(sp)
50000114:	46c2                	lw	a3,16(sp)
50000116:	4752                	lw	a4,20(sp)
50000118:	47e2                	lw	a5,24(sp)
5000011a:	45a2                	lw	a1,8(sp)
5000011c:	c188                	sw	a0,0(a1)
5000011e:	4082                	lw	ra,0(sp)
50000120:	a011                	j	50000124 <readbarrier_end_read_barrier>

50000122 <readbarrier_translation_skip>:
50000122:	4572                	lw	a0,28(sp)

50000124 <readbarrier_end_read_barrier>:
50000124:	00051583          	lh	a1,0(a0)
50000128:	a801                	j	50000138 <readbarrier_return>

5000012a <readbarrier_this_is_nil>:
5000012a:	4585                	li	a1,1
5000012c:	a031                	j	50000138 <readbarrier_return>

5000012e <readbarrier_this_is_bool>:
5000012e:	458d                	li	a1,3
50000130:	8509                	srai	a0,a0,0x2
50000132:	a019                	j	50000138 <readbarrier_return>

50000134 <readbarrier_this_is_int>:
50000134:	4591                	li	a1,4
50000136:	8505                	srai	a0,a0,0x1

50000138 <readbarrier_return>:
50000138:	81aa                	mv	gp,a0
5000013a:	4512                	lw	a0,4(sp)
5000013c:	6105                	addi	sp,sp,32
5000013e:	8082                	ret

50000140 <object_new>:
50000140:	7179                	addi	sp,sp,-48
50000142:	c006                	sw	ra,0(sp)
50000144:	c22a                	sw	a0,4(sp)
50000146:	c42e                	sw	a1,8(sp)
50000148:	c632                	sw	a2,12(sp)
5000014a:	c836                	sw	a3,16(sp)
5000014c:	ca3a                	sw	a4,20(sp)
5000014e:	cc3e                	sw	a5,24(sp)
50000150:	ce42                	sw	a6,28(sp)
50000152:	d046                	sw	a7,32(sp)
50000154:	8141                	srli	a0,a0,0x10
50000156:	050a                	slli	a0,a0,0x2
50000158:	0511                	addi	a0,a0,4
5000015a:	1e3000ef          	jal	50000b3c <mrbc_gc_alloc>
5000015e:	4592                	lw	a1,4(sp)
50000160:	c10c                	sw	a1,0(a0)
50000162:	4082                	lw	ra,0(sp)
50000164:	45a2                	lw	a1,8(sp)
50000166:	4632                	lw	a2,12(sp)
50000168:	46c2                	lw	a3,16(sp)
5000016a:	4752                	lw	a4,20(sp)
5000016c:	47e2                	lw	a5,24(sp)
5000016e:	4872                	lw	a6,28(sp)
50000170:	5882                	lw	a7,32(sp)
50000172:	6145                	addi	sp,sp,48
50000174:	8082                	ret

50000176 <sleep_store>:
50000176:	7139                	addi	sp,sp,-64
50000178:	c006                	sw	ra,0(sp)
5000017a:	c20e                	sw	gp,4(sp)
5000017c:	c412                	sw	tp,8(sp)
5000017e:	c822                	sw	s0,16(sp)
50000180:	ca26                	sw	s1,20(sp)
50000182:	cc4a                	sw	s2,24(sp)
50000184:	ce4e                	sw	s3,28(sp)
50000186:	d052                	sw	s4,32(sp)
50000188:	d256                	sw	s5,36(sp)
5000018a:	036128a3          	sw	s6,49(sp)
5000018e:	d65e                	sw	s7,44(sp)
50000190:	d862                	sw	s8,48(sp)
50000192:	da66                	sw	s9,52(sp)
50000194:	dc6a                	sw	s10,56(sp)
50000196:	de6e                	sw	s11,60(sp)
50000198:	00002517          	auipc	a0,0x2
5000019c:	43450513          	addi	a0,a0,1076 # 500025cc <mrbc_sp_bottom>
500001a0:	00252023          	sw	sp,0(a0)
500001a4:	3f00006f          	j	50000594 <sleep_store2>

500001a8 <sleep_restore>:
500001a8:	00002517          	auipc	a0,0x2
500001ac:	42450513          	addi	a0,a0,1060 # 500025cc <mrbc_sp_bottom>
500001b0:	00052103          	lw	sp,0(a0)
500001b4:	4082                	lw	ra,0(sp)
500001b6:	4192                	lw	gp,4(sp)
500001b8:	4222                	lw	tp,8(sp)
500001ba:	4442                	lw	s0,16(sp)
500001bc:	44d2                	lw	s1,20(sp)
500001be:	4962                	lw	s2,24(sp)
500001c0:	49f2                	lw	s3,28(sp)
500001c2:	5a02                	lw	s4,32(sp)
500001c4:	5a92                	lw	s5,36(sp)
500001c6:	03112b03          	lw	s6,49(sp)
500001ca:	5bb2                	lw	s7,44(sp)
500001cc:	5c42                	lw	s8,48(sp)
500001ce:	5cd2                	lw	s9,52(sp)
500001d0:	5d62                	lw	s10,56(sp)
500001d2:	5df2                	lw	s11,60(sp)
500001d4:	6121                	addi	sp,sp,64
500001d6:	00008067          	ret

500001da <array_get>:
500001da:	00251603          	lh	a2,2(a0)
500001de:	00c5e563          	bltu	a1,a2,500001e8 <array_get_ok>
500001e2:	4501                	li	a0,0
500001e4:	4585                	li	a1,1
500001e6:	8082                	ret

500001e8 <array_get_ok>:
500001e8:	058a                	slli	a1,a1,0x2
500001ea:	95aa                	add	a1,a1,a0
500001ec:	0591                	addi	a1,a1,4 # 4004 <RvExcFrameSize+0x3f70>
500001ee:	1141                	addi	sp,sp,-16
500001f0:	c006                	sw	ra,0(sp)
500001f2:	35ed                	jal	500000dc <read_barrier>
500001f4:	4082                	lw	ra,0(sp)
500001f6:	0141                	addi	sp,sp,16
500001f8:	850e                	mv	a0,gp
500001fa:	8082                	ret

500001fc <string_getbyte>:
500001fc:	00251603          	lh	a2,2(a0)
50000200:	00c5e563          	bltu	a1,a2,5000020a <string_getbyte_ok>
50000204:	4501                	li	a0,0
50000206:	4585                	li	a1,1
50000208:	8082                	ret

5000020a <string_getbyte_ok>:
5000020a:	95aa                	add	a1,a1,a0
5000020c:	0045c503          	lbu	a0,4(a1)
50000210:	4591                	li	a1,4
50000212:	8082                	ret

50000214 <array_set>:
50000214:	00251683          	lh	a3,2(a0)
50000218:	00d5e363          	bltu	a1,a3,5000021e <array_set_ok>
5000021c:	8082                	ret

5000021e <array_set_ok>:
5000021e:	058a                	slli	a1,a1,0x2
50000220:	95aa                	add	a1,a1,a0
50000222:	0591                	addi	a1,a1,4
50000224:	c190                	sw	a2,0(a1)
50000226:	8532                	mv	a0,a2
50000228:	8082                	ret

5000022a <__udivdi3>:
5000022a:	88aa                	mv	a7,a0
5000022c:	832e                	mv	t1,a1
5000022e:	8732                	mv	a4,a2
50000230:	882a                	mv	a6,a0
50000232:	87ae                	mv	a5,a1
50000234:	20069663          	bnez	a3,50000440 <__udivdi3+0x216>
50000238:	500015b7          	lui	a1,0x50001
5000023c:	45858593          	addi	a1,a1,1112 # 50001458 <__clz_tab>
50000240:	0cc37163          	bgeu	t1,a2,50000302 <__udivdi3+0xd8>
50000244:	66c1                	lui	a3,0x10
50000246:	0ad67763          	bgeu	a2,a3,500002f4 <__udivdi3+0xca>
5000024a:	10063693          	sltiu	a3,a2,256
5000024e:	0016b693          	seqz	a3,a3
50000252:	068e                	slli	a3,a3,0x3
50000254:	00d65533          	srl	a0,a2,a3
50000258:	95aa                	add	a1,a1,a0
5000025a:	0005c583          	lbu	a1,0(a1)
5000025e:	02000513          	li	a0,32
50000262:	96ae                	add	a3,a3,a1
50000264:	40d505b3          	sub	a1,a0,a3
50000268:	00d50b63          	beq	a0,a3,5000027e <__udivdi3+0x54>
5000026c:	00b317b3          	sll	a5,t1,a1
50000270:	00d8d6b3          	srl	a3,a7,a3
50000274:	00b61733          	sll	a4,a2,a1
50000278:	8fd5                	or	a5,a5,a3
5000027a:	00b89833          	sll	a6,a7,a1
5000027e:	01075593          	srli	a1,a4,0x10
50000282:	02b7d333          	divu	t1,a5,a1
50000286:	01071613          	slli	a2,a4,0x10
5000028a:	8241                	srli	a2,a2,0x10
5000028c:	02b7f7b3          	remu	a5,a5,a1
50000290:	851a                	mv	a0,t1
50000292:	026608b3          	mul	a7,a2,t1
50000296:	01079693          	slli	a3,a5,0x10
5000029a:	01085793          	srli	a5,a6,0x10
5000029e:	8fd5                	or	a5,a5,a3
500002a0:	0117fc63          	bgeu	a5,a7,500002b8 <__udivdi3+0x8e>
500002a4:	97ba                	add	a5,a5,a4
500002a6:	fff30513          	addi	a0,t1,-1
500002aa:	00e7e763          	bltu	a5,a4,500002b8 <__udivdi3+0x8e>
500002ae:	0117f563          	bgeu	a5,a7,500002b8 <__udivdi3+0x8e>
500002b2:	ffe30513          	addi	a0,t1,-2
500002b6:	97ba                	add	a5,a5,a4
500002b8:	411787b3          	sub	a5,a5,a7
500002bc:	02b7d8b3          	divu	a7,a5,a1
500002c0:	0842                	slli	a6,a6,0x10
500002c2:	01085813          	srli	a6,a6,0x10
500002c6:	02b7f7b3          	remu	a5,a5,a1
500002ca:	031606b3          	mul	a3,a2,a7
500002ce:	07c2                	slli	a5,a5,0x10
500002d0:	00f86833          	or	a6,a6,a5
500002d4:	87c6                	mv	a5,a7
500002d6:	00d87b63          	bgeu	a6,a3,500002ec <__udivdi3+0xc2>
500002da:	983a                	add	a6,a6,a4
500002dc:	fff88793          	addi	a5,a7,-1
500002e0:	00e86663          	bltu	a6,a4,500002ec <__udivdi3+0xc2>
500002e4:	00d87463          	bgeu	a6,a3,500002ec <__udivdi3+0xc2>
500002e8:	ffe88793          	addi	a5,a7,-2
500002ec:	0542                	slli	a0,a0,0x10
500002ee:	8d5d                	or	a0,a0,a5
500002f0:	4581                	li	a1,0
500002f2:	8082                	ret
500002f4:	01000537          	lui	a0,0x1000
500002f8:	46e1                	li	a3,24
500002fa:	f4a67de3          	bgeu	a2,a0,50000254 <__udivdi3+0x2a>
500002fe:	46c1                	li	a3,16
50000300:	bf91                	j	50000254 <__udivdi3+0x2a>
50000302:	4681                	li	a3,0
50000304:	ca09                	beqz	a2,50000316 <__udivdi3+0xec>
50000306:	67c1                	lui	a5,0x10
50000308:	08f67f63          	bgeu	a2,a5,500003a6 <__udivdi3+0x17c>
5000030c:	10063693          	sltiu	a3,a2,256
50000310:	0016b693          	seqz	a3,a3
50000314:	068e                	slli	a3,a3,0x3
50000316:	00d657b3          	srl	a5,a2,a3
5000031a:	95be                	add	a1,a1,a5
5000031c:	0005c783          	lbu	a5,0(a1)
50000320:	97b6                	add	a5,a5,a3
50000322:	02000693          	li	a3,32
50000326:	40f685b3          	sub	a1,a3,a5
5000032a:	08f69563          	bne	a3,a5,500003b4 <__udivdi3+0x18a>
5000032e:	40c307b3          	sub	a5,t1,a2
50000332:	4585                	li	a1,1
50000334:	01075893          	srli	a7,a4,0x10
50000338:	0317de33          	divu	t3,a5,a7
5000033c:	01071613          	slli	a2,a4,0x10
50000340:	8241                	srli	a2,a2,0x10
50000342:	01085693          	srli	a3,a6,0x10
50000346:	0317f7b3          	remu	a5,a5,a7
5000034a:	8572                	mv	a0,t3
5000034c:	03c60333          	mul	t1,a2,t3
50000350:	07c2                	slli	a5,a5,0x10
50000352:	8fd5                	or	a5,a5,a3
50000354:	0067fc63          	bgeu	a5,t1,5000036c <__udivdi3+0x142>
50000358:	97ba                	add	a5,a5,a4
5000035a:	fffe0513          	addi	a0,t3,-1
5000035e:	00e7e763          	bltu	a5,a4,5000036c <__udivdi3+0x142>
50000362:	0067f563          	bgeu	a5,t1,5000036c <__udivdi3+0x142>
50000366:	ffee0513          	addi	a0,t3,-2
5000036a:	97ba                	add	a5,a5,a4
5000036c:	406787b3          	sub	a5,a5,t1
50000370:	0317d333          	divu	t1,a5,a7
50000374:	0842                	slli	a6,a6,0x10
50000376:	01085813          	srli	a6,a6,0x10
5000037a:	0317f7b3          	remu	a5,a5,a7
5000037e:	026606b3          	mul	a3,a2,t1
50000382:	07c2                	slli	a5,a5,0x10
50000384:	00f86833          	or	a6,a6,a5
50000388:	879a                	mv	a5,t1
5000038a:	00d87b63          	bgeu	a6,a3,500003a0 <__udivdi3+0x176>
5000038e:	983a                	add	a6,a6,a4
50000390:	fff30793          	addi	a5,t1,-1
50000394:	00e86663          	bltu	a6,a4,500003a0 <__udivdi3+0x176>
50000398:	00d87463          	bgeu	a6,a3,500003a0 <__udivdi3+0x176>
5000039c:	ffe30793          	addi	a5,t1,-2
500003a0:	0542                	slli	a0,a0,0x10
500003a2:	8d5d                	or	a0,a0,a5
500003a4:	8082                	ret
500003a6:	010007b7          	lui	a5,0x1000
500003aa:	46e1                	li	a3,24
500003ac:	f6f675e3          	bgeu	a2,a5,50000316 <__udivdi3+0xec>
500003b0:	46c1                	li	a3,16
500003b2:	b795                	j	50000316 <__udivdi3+0xec>
500003b4:	00b61733          	sll	a4,a2,a1
500003b8:	00f356b3          	srl	a3,t1,a5
500003bc:	01075513          	srli	a0,a4,0x10
500003c0:	00b31333          	sll	t1,t1,a1
500003c4:	00f8d7b3          	srl	a5,a7,a5
500003c8:	0067e7b3          	or	a5,a5,t1
500003cc:	02a6d333          	divu	t1,a3,a0
500003d0:	01071613          	slli	a2,a4,0x10
500003d4:	8241                	srli	a2,a2,0x10
500003d6:	00b89833          	sll	a6,a7,a1
500003da:	02a6f6b3          	remu	a3,a3,a0
500003de:	026608b3          	mul	a7,a2,t1
500003e2:	01069593          	slli	a1,a3,0x10
500003e6:	0107d693          	srli	a3,a5,0x10
500003ea:	8ecd                	or	a3,a3,a1
500003ec:	859a                	mv	a1,t1
500003ee:	0116fc63          	bgeu	a3,a7,50000406 <__udivdi3+0x1dc>
500003f2:	96ba                	add	a3,a3,a4
500003f4:	fff30593          	addi	a1,t1,-1
500003f8:	00e6e763          	bltu	a3,a4,50000406 <__udivdi3+0x1dc>
500003fc:	0116f563          	bgeu	a3,a7,50000406 <__udivdi3+0x1dc>
50000400:	ffe30593          	addi	a1,t1,-2
50000404:	96ba                	add	a3,a3,a4
50000406:	411686b3          	sub	a3,a3,a7
5000040a:	02a6d8b3          	divu	a7,a3,a0
5000040e:	07c2                	slli	a5,a5,0x10
50000410:	83c1                	srli	a5,a5,0x10
50000412:	02a6f6b3          	remu	a3,a3,a0
50000416:	03160633          	mul	a2,a2,a7
5000041a:	06c2                	slli	a3,a3,0x10
5000041c:	8fd5                	or	a5,a5,a3
5000041e:	86c6                	mv	a3,a7
50000420:	00c7fc63          	bgeu	a5,a2,50000438 <__udivdi3+0x20e>
50000424:	97ba                	add	a5,a5,a4
50000426:	fff88693          	addi	a3,a7,-1
5000042a:	00e7e763          	bltu	a5,a4,50000438 <__udivdi3+0x20e>
5000042e:	00c7f563          	bgeu	a5,a2,50000438 <__udivdi3+0x20e>
50000432:	ffe88693          	addi	a3,a7,-2
50000436:	97ba                	add	a5,a5,a4
50000438:	05c2                	slli	a1,a1,0x10
5000043a:	8f91                	sub	a5,a5,a2
5000043c:	8dd5                	or	a1,a1,a3
5000043e:	bddd                	j	50000334 <__udivdi3+0x10a>
50000440:	12d5ef63          	bltu	a1,a3,5000057e <__udivdi3+0x354>
50000444:	67c1                	lui	a5,0x10
50000446:	02f6ff63          	bgeu	a3,a5,50000484 <__udivdi3+0x25a>
5000044a:	1006b793          	sltiu	a5,a3,256
5000044e:	0017b793          	seqz	a5,a5
50000452:	078e                	slli	a5,a5,0x3
50000454:	50001737          	lui	a4,0x50001
50000458:	00f6d5b3          	srl	a1,a3,a5
5000045c:	45870713          	addi	a4,a4,1112 # 50001458 <__clz_tab>
50000460:	972e                	add	a4,a4,a1
50000462:	00074703          	lbu	a4,0(a4)
50000466:	973e                	add	a4,a4,a5
50000468:	02000793          	li	a5,32
5000046c:	40e785b3          	sub	a1,a5,a4
50000470:	02e79163          	bne	a5,a4,50000492 <__udivdi3+0x268>
50000474:	4505                	li	a0,1
50000476:	e666eee3          	bltu	a3,t1,500002f2 <__udivdi3+0xc8>
5000047a:	00c8b533          	sltu	a0,a7,a2
5000047e:	00153513          	seqz	a0,a0
50000482:	8082                	ret
50000484:	01000737          	lui	a4,0x1000
50000488:	47e1                	li	a5,24
5000048a:	fce6f5e3          	bgeu	a3,a4,50000454 <__udivdi3+0x22a>
5000048e:	47c1                	li	a5,16
50000490:	b7d1                	j	50000454 <__udivdi3+0x22a>
50000492:	00e65533          	srl	a0,a2,a4
50000496:	00b696b3          	sll	a3,a3,a1
5000049a:	00e357b3          	srl	a5,t1,a4
5000049e:	8d55                	or	a0,a0,a3
500004a0:	00b31333          	sll	t1,t1,a1
500004a4:	00e8d733          	srl	a4,a7,a4
500004a8:	00676733          	or	a4,a4,t1
500004ac:	01055313          	srli	t1,a0,0x10
500004b0:	0267deb3          	divu	t4,a5,t1
500004b4:	01051813          	slli	a6,a0,0x10
500004b8:	01085813          	srli	a6,a6,0x10
500004bc:	01075693          	srli	a3,a4,0x10
500004c0:	00b61633          	sll	a2,a2,a1
500004c4:	0267f7b3          	remu	a5,a5,t1
500004c8:	03d80e33          	mul	t3,a6,t4
500004cc:	07c2                	slli	a5,a5,0x10
500004ce:	8edd                	or	a3,a3,a5
500004d0:	87f6                	mv	a5,t4
500004d2:	01c6fc63          	bgeu	a3,t3,500004ea <__udivdi3+0x2c0>
500004d6:	96aa                	add	a3,a3,a0
500004d8:	fffe8793          	addi	a5,t4,-1
500004dc:	00a6e763          	bltu	a3,a0,500004ea <__udivdi3+0x2c0>
500004e0:	01c6f563          	bgeu	a3,t3,500004ea <__udivdi3+0x2c0>
500004e4:	ffee8793          	addi	a5,t4,-2
500004e8:	96aa                	add	a3,a3,a0
500004ea:	41c686b3          	sub	a3,a3,t3
500004ee:	0266de33          	divu	t3,a3,t1
500004f2:	0742                	slli	a4,a4,0x10
500004f4:	8341                	srli	a4,a4,0x10
500004f6:	0266f6b3          	remu	a3,a3,t1
500004fa:	03c80833          	mul	a6,a6,t3
500004fe:	06c2                	slli	a3,a3,0x10
50000500:	8f55                	or	a4,a4,a3
50000502:	86f2                	mv	a3,t3
50000504:	01077c63          	bgeu	a4,a6,5000051c <__udivdi3+0x2f2>
50000508:	972a                	add	a4,a4,a0
5000050a:	fffe0693          	addi	a3,t3,-1
5000050e:	00a76763          	bltu	a4,a0,5000051c <__udivdi3+0x2f2>
50000512:	01077563          	bgeu	a4,a6,5000051c <__udivdi3+0x2f2>
50000516:	ffee0693          	addi	a3,t3,-2
5000051a:	972a                	add	a4,a4,a0
5000051c:	07c2                	slli	a5,a5,0x10
5000051e:	00d7e533          	or	a0,a5,a3
50000522:	01061313          	slli	t1,a2,0x10
50000526:	06c2                	slli	a3,a3,0x10
50000528:	82c1                	srli	a3,a3,0x10
5000052a:	01035313          	srli	t1,t1,0x10
5000052e:	8241                	srli	a2,a2,0x10
50000530:	41070733          	sub	a4,a4,a6
50000534:	01055813          	srli	a6,a0,0x10
50000538:	02668e33          	mul	t3,a3,t1
5000053c:	02680333          	mul	t1,a6,t1
50000540:	010e5793          	srli	a5,t3,0x10
50000544:	02c686b3          	mul	a3,a3,a2
50000548:	969a                	add	a3,a3,t1
5000054a:	97b6                	add	a5,a5,a3
5000054c:	02c80833          	mul	a6,a6,a2
50000550:	0067f463          	bgeu	a5,t1,50000558 <__udivdi3+0x32e>
50000554:	66c1                	lui	a3,0x10
50000556:	9836                	add	a6,a6,a3
50000558:	0107d693          	srli	a3,a5,0x10
5000055c:	96c2                	add	a3,a3,a6
5000055e:	00d76e63          	bltu	a4,a3,5000057a <__udivdi3+0x350>
50000562:	d8d717e3          	bne	a4,a3,500002f0 <__udivdi3+0xc6>
50000566:	0e42                	slli	t3,t3,0x10
50000568:	07c2                	slli	a5,a5,0x10
5000056a:	010e5e13          	srli	t3,t3,0x10
5000056e:	00b898b3          	sll	a7,a7,a1
50000572:	97f2                	add	a5,a5,t3
50000574:	4581                	li	a1,0
50000576:	d6f8fee3          	bgeu	a7,a5,500002f2 <__udivdi3+0xc8>
5000057a:	157d                	addi	a0,a0,-1 # ffffff <RvExcFrameSize+0xffff6b>
5000057c:	bb95                	j	500002f0 <__udivdi3+0xc6>
5000057e:	4581                	li	a1,0
50000580:	4501                	li	a0,0
50000582:	8082                	ret

50000584 <ulp_lp_core_lp_timer_intr_handler>:
50000584:	1141                	addi	sp,sp,-16
50000586:	c606                	sw	ra,12(sp)
50000588:	2fb000ef          	jal	50001082 <ulp_lp_core_lp_timer_intr_clear>
5000058c:	40b2                	lw	ra,12(sp)
5000058e:	0141                	addi	sp,sp,16
50000590:	0ef0006f          	j	50000e7e <ulp_lp_core_lp_timer_disable>

50000594 <sleep_store2>:
50000594:	1141                	addi	sp,sp,-16
50000596:	c606                	sw	ra,12(sp)
50000598:	299000ef          	jal	50001030 <ulp_lp_core_halt>

5000059c <copro_delayUs>:
5000059c:	03100793          	li	a5,49
500005a0:	852e                	mv	a0,a1
500005a2:	00b7c463          	blt	a5,a1,500005aa <copro_delayUs+0xe>
500005a6:	2690006f          	j	5000100e <ulp_lp_core_delay_us>
500005aa:	1101                	addi	sp,sp,-32
500005ac:	85fd                	srai	a1,a1,0x1f
500005ae:	c62a                	sw	a0,12(sp)
500005b0:	ce06                	sw	ra,28(sp)
500005b2:	061000ef          	jal	50000e12 <ulp_lp_core_lp_timer_set_wakeup_time>
500005b6:	4532                	lw	a0,12(sp)
500005b8:	3e700793          	li	a5,999
500005bc:	02a7c163          	blt	a5,a0,500005de <copro_delayUs+0x42>
500005c0:	300467f3          	csrrsi	a5,mstatus,8
500005c4:	400007b7          	lui	a5,0x40000
500005c8:	3047a773          	csrrs	a4,mie,a5
500005cc:	10500073          	wfi
500005d0:	30047773          	csrrci	a4,mstatus,8
500005d4:	3047b7f3          	csrrc	a5,mie,a5
500005d8:	40f2                	lw	ra,28(sp)
500005da:	6105                	addi	sp,sp,32
500005dc:	8082                	ret
500005de:	600b07b7          	lui	a5,0x600b0
500005e2:	00078793          	mv	a5,a5
500005e6:	4741                	li	a4,16
500005e8:	18e79023          	sh	a4,384(a5) # 600b0180 <PMU+0x180>
500005ec:	500007b7          	lui	a5,0x50000
500005f0:	40f2                	lw	ra,28(sp)
500005f2:	50001737          	lui	a4,0x50001
500005f6:	1a878793          	addi	a5,a5,424 # 500001a8 <sleep_restore>
500005fa:	58f72c23          	sw	a5,1432(a4) # 50001598 <entrypoint>
500005fe:	6105                	addi	sp,sp,32
50000600:	be9d                	j	50000176 <sleep_store>

50000602 <copro_delayMs>:
50000602:	3e800793          	li	a5,1000
50000606:	02f585b3          	mul	a1,a1,a5
5000060a:	bf49                	j	5000059c <copro_delayUs>

5000060c <copro_GPIOpulseIn>:
5000060c:	1141                	addi	sp,sp,-16
5000060e:	4501                	li	a0,0
50000610:	00c548b3          	xor	a7,a0,a2
50000614:	b0002873          	csrr	a6,mcycle
50000618:	e28d                	bnez	a3,5000063a <copro_GPIOpulseIn+0x2e>
5000061a:	600b2737          	lui	a4,0x600b2
5000061e:	00070793          	mv	a5,a4
50000622:	53dc                	lw	a5,36(a5)
50000624:	c43e                	sw	a5,8(sp)
50000626:	00814783          	lbu	a5,8(sp)
5000062a:	40b7d7b3          	sra	a5,a5,a1
5000062e:	8b85                	andi	a5,a5,1
50000630:	fef897e3          	bne	a7,a5,5000061e <copro_GPIOpulseIn+0x12>
50000634:	ed05                	bnez	a0,5000066c <copro_GPIOpulseIn+0x60>
50000636:	4505                	li	a0,1
50000638:	bfe1                	j	50000610 <copro_GPIOpulseIn+0x4>
5000063a:	00469793          	slli	a5,a3,0x4
5000063e:	17b1                	addi	a5,a5,-20
50000640:	97c2                	add	a5,a5,a6
50000642:	600b2337          	lui	t1,0x600b2
50000646:	b0002773          	csrr	a4,mcycle
5000064a:	00f76563          	bltu	a4,a5,50000654 <copro_GPIOpulseIn+0x48>
5000064e:	4501                	li	a0,0
50000650:	0141                	addi	sp,sp,16
50000652:	8082                	ret
50000654:	00030713          	mv	a4,t1
50000658:	5358                	lw	a4,36(a4)
5000065a:	c63a                	sw	a4,12(sp)
5000065c:	00c14703          	lbu	a4,12(sp)
50000660:	40b75733          	sra	a4,a4,a1
50000664:	8b05                	andi	a4,a4,1
50000666:	fee890e3          	bne	a7,a4,50000646 <copro_GPIOpulseIn+0x3a>
5000066a:	b7e9                	j	50000634 <copro_GPIOpulseIn+0x28>
5000066c:	b0002573          	csrr	a0,mcycle
50000670:	41050533          	sub	a0,a0,a6
50000674:	8111                	srli	a0,a0,0x4
50000676:	bfe9                	j	50000650 <copro_GPIOpulseIn+0x44>

50000678 <debugout>:
50000678:	1141                	addi	sp,sp,-16
5000067a:	500027b7          	lui	a5,0x50002
5000067e:	c422                	sw	s0,8(sp)
50000680:	5ca7a223          	sw	a0,1476(a5) # 500025c4 <required_object>
50000684:	50002437          	lui	s0,0x50002
50000688:	4789                	li	a5,2
5000068a:	c226                	sw	s1,4(sp)
5000068c:	c606                	sw	ra,12(sp)
5000068e:	5cf41523          	sh	a5,1482(s0) # 500025ca <stopreason>
50000692:	500024b7          	lui	s1,0x50002
50000696:	165000ef          	jal	50000ffa <ulp_lp_core_wakeup_main_processor>
5000069a:	5c84d783          	lhu	a5,1480(s1) # 500025c8 <ack>
5000069e:	dfe5                	beqz	a5,50000696 <debugout+0x1e>
500006a0:	3e800513          	li	a0,1000
500006a4:	16b000ef          	jal	5000100e <ulp_lp_core_delay_us>
500006a8:	47a9                	li	a5,10
500006aa:	5c049423          	sh	zero,1480(s1)
500006ae:	40b2                	lw	ra,12(sp)
500006b0:	5cf41523          	sh	a5,1482(s0)
500006b4:	4422                	lw	s0,8(sp)
500006b6:	4492                	lw	s1,4(sp)
500006b8:	0141                	addi	sp,sp,16
500006ba:	8082                	ret

500006bc <fallback_post>:
500006bc:	1141                	addi	sp,sp,-16
500006be:	c606                	sw	ra,12(sp)
500006c0:	c422                	sw	s0,8(sp)
500006c2:	0818                	addi	a4,sp,16
500006c4:	500027b7          	lui	a5,0x50002
500006c8:	5ce7a623          	sw	a4,1484(a5) # 500025cc <mrbc_sp_bottom>
500006cc:	500027b7          	lui	a5,0x50002
500006d0:	5c079523          	sh	zero,1482(a5) # 500025ca <stopreason>
500006d4:	50002437          	lui	s0,0x50002
500006d8:	123000ef          	jal	50000ffa <ulp_lp_core_wakeup_main_processor>
500006dc:	5c845783          	lhu	a5,1480(s0) # 500025c8 <ack>
500006e0:	dfe5                	beqz	a5,500006d8 <fallback_post+0x1c>
500006e2:	79c000ef          	jal	50000e7e <ulp_lp_core_lp_timer_disable>
500006e6:	14b000ef          	jal	50001030 <ulp_lp_core_halt>

500006ea <copro_GPIOget>:
500006ea:	600b27b7          	lui	a5,0x600b2
500006ee:	00078793          	mv	a5,a5
500006f2:	53dc                	lw	a5,36(a5)
500006f4:	1141                	addi	sp,sp,-16
500006f6:	c63e                	sw	a5,12(sp)
500006f8:	00c14503          	lbu	a0,12(sp)
500006fc:	0141                	addi	sp,sp,16
500006fe:	40b55533          	sra	a0,a0,a1
50000702:	8905                	andi	a0,a0,1
50000704:	8082                	ret

50000706 <copro_GPIOset>:
50000706:	4785                	li	a5,1
50000708:	00b795b3          	sll	a1,a5,a1
5000070c:	0ff67613          	zext.b	a2,a2
50000710:	600b27b7          	lui	a5,0x600b2
50000714:	1141                	addi	sp,sp,-16
50000716:	0ff5f593          	zext.b	a1,a1
5000071a:	00078793          	mv	a5,a5
5000071e:	ca09                	beqz	a2,50000730 <copro_GPIOset+0x2a>
50000720:	43d8                	lw	a4,4(a5)
50000722:	c43a                	sw	a4,8(sp)
50000724:	00b10423          	sb	a1,8(sp)
50000728:	4722                	lw	a4,8(sp)
5000072a:	c3d8                	sw	a4,4(a5)
5000072c:	0141                	addi	sp,sp,16
5000072e:	8082                	ret
50000730:	4798                	lw	a4,8(a5)
50000732:	c63a                	sw	a4,12(sp)
50000734:	00b10623          	sb	a1,12(sp)
50000738:	4732                	lw	a4,12(sp)
5000073a:	c798                	sw	a4,8(a5)
5000073c:	bfc5                	j	5000072c <copro_GPIOset+0x26>

5000073e <copro_I2Cread>:
5000073e:	1101                	addi	sp,sp,-32
50000740:	00460513          	addi	a0,a2,4
50000744:	cc22                	sw	s0,24(sp)
50000746:	ca26                	sw	s1,20(sp)
50000748:	ce06                	sw	ra,28(sp)
5000074a:	84ae                	mv	s1,a1
5000074c:	c632                	sw	a2,12(sp)
5000074e:	26fd                	jal	50000b3c <mrbc_gc_alloc>
50000750:	46b2                	lw	a3,12(sp)
50000752:	47b1                	li	a5,12
50000754:	01049593          	slli	a1,s1,0x10
50000758:	842a                	mv	s0,a0
5000075a:	00d51123          	sh	a3,2(a0)
5000075e:	00f51023          	sh	a5,0(a0)
50000762:	00450613          	addi	a2,a0,4
50000766:	577d                	li	a4,-1
50000768:	81c1                	srli	a1,a1,0x10
5000076a:	4505                	li	a0,1
5000076c:	253000ef          	jal	500011be <lp_core_i2c_master_read_from_device>
50000770:	c119                	beqz	a0,50000776 <copro_I2Cread+0x38>
50000772:	00041123          	sh	zero,2(s0)
50000776:	40f2                	lw	ra,28(sp)
50000778:	8522                	mv	a0,s0
5000077a:	4462                	lw	s0,24(sp)
5000077c:	44d2                	lw	s1,20(sp)
5000077e:	6105                	addi	sp,sp,32
50000780:	8082                	ret

50000782 <copro_I2Cwrite>:
50000782:	1141                	addi	sp,sp,-16
50000784:	c422                	sw	s0,8(sp)
50000786:	c606                	sw	ra,12(sp)
50000788:	c226                	sw	s1,4(sp)
5000078a:	0800                	addi	s0,sp,16
5000078c:	00065703          	lhu	a4,0(a2)
50000790:	46b1                	li	a3,12
50000792:	87b2                	mv	a5,a2
50000794:	00d71e63          	bne	a4,a3,500007b0 <copro_I2Cwrite+0x2e>
50000798:	0027d483          	lhu	s1,2(a5) # 600b2002 <LP_IO+0x2>
5000079c:	0611                	addi	a2,a2,4
5000079e:	05c2                	slli	a1,a1,0x10
500007a0:	577d                	li	a4,-1
500007a2:	86a6                	mv	a3,s1
500007a4:	81c1                	srli	a1,a1,0x10
500007a6:	4505                	li	a0,1
500007a8:	359000ef          	jal	50001300 <lp_core_i2c_master_write_to_device>
500007ac:	e90d                	bnez	a0,500007de <copro_I2Cwrite+0x5c>
500007ae:	a80d                	j	500007e0 <copro_I2Cwrite+0x5e>
500007b0:	46ad                	li	a3,11
500007b2:	02d71663          	bne	a4,a3,500007de <copro_I2Cwrite+0x5c>
500007b6:	00265483          	lhu	s1,2(a2)
500007ba:	0791                	addi	a5,a5,4
500007bc:	01748713          	addi	a4,s1,23
500007c0:	8311                	srli	a4,a4,0x4
500007c2:	0712                	slli	a4,a4,0x4
500007c4:	40e10133          	sub	sp,sp,a4
500007c8:	00f10613          	addi	a2,sp,15
500007cc:	9a41                	andi	a2,a2,-16
500007ce:	4701                	li	a4,0
500007d0:	fc9757e3          	bge	a4,s1,5000079e <copro_I2Cwrite+0x1c>
500007d4:	4394                	lw	a3,0(a5)
500007d6:	0791                	addi	a5,a5,4
500007d8:	0016f513          	andi	a0,a3,1
500007dc:	e911                	bnez	a0,500007f0 <copro_I2Cwrite+0x6e>
500007de:	4481                	li	s1,0
500007e0:	ff040113          	addi	sp,s0,-16
500007e4:	40b2                	lw	ra,12(sp)
500007e6:	8526                	mv	a0,s1
500007e8:	4422                	lw	s0,8(sp)
500007ea:	4492                	lw	s1,4(sp)
500007ec:	0141                	addi	sp,sp,16
500007ee:	8082                	ret
500007f0:	00e60533          	add	a0,a2,a4
500007f4:	8285                	srli	a3,a3,0x1
500007f6:	00d50023          	sb	a3,0(a0)
500007fa:	0705                	addi	a4,a4,1 # 600b2001 <LP_IO+0x1>
500007fc:	bfd1                	j	500007d0 <copro_I2Cwrite+0x4e>

500007fe <imalive>:
500007fe:	1141                	addi	sp,sp,-16
50000800:	c606                	sw	ra,12(sp)
50000802:	3d9d                	jal	50000678 <debugout>
50000804:	3861                	jal	5000009c <fallback>
50000806:	38d9                	jal	500000dc <read_barrier>
50000808:	2e15                	jal	50000b3c <mrbc_gc_alloc>
5000080a:	35c5                	jal	500006ea <copro_GPIOget>
5000080c:	3ded                	jal	50000706 <copro_GPIOset>
5000080e:	3f05                	jal	5000073e <copro_I2Cread>
50000810:	3f8d                	jal	50000782 <copro_I2Cwrite>
50000812:	3bc5                	jal	50000602 <copro_delayMs>
50000814:	32d9                	jal	500001da <array_get>
50000816:	32dd                	jal	500001fc <string_getbyte>
50000818:	3af5                	jal	50000214 <array_set>
5000081a:	40b2                	lw	ra,12(sp)
5000081c:	0141                	addi	sp,sp,16
5000081e:	b3fd                	j	5000060c <copro_GPIOpulseIn>

50000820 <readfail>:
50000820:	1101                	addi	sp,sp,-32
50000822:	cc22                	sw	s0,24(sp)
50000824:	ca26                	sw	s1,20(sp)
50000826:	c64e                	sw	s3,12(sp)
50000828:	c452                	sw	s4,8(sp)
5000082a:	c256                	sw	s5,4(sp)
5000082c:	c05a                	sw	s6,0(sp)
5000082e:	ce06                	sw	ra,28(sp)
50000830:	c84a                	sw	s2,16(sp)
50000832:	842a                	mv	s0,a0
50000834:	500019b7          	lui	s3,0x50001
50000838:	4a11                	li	s4,4
5000083a:	500024b7          	lui	s1,0x50002
5000083e:	4a85                	li	s5,1
50000840:	50002b37          	lui	s6,0x50002
50000844:	4781                	li	a5,0
50000846:	00379693          	slli	a3,a5,0x3
5000084a:	5a498713          	addi	a4,s3,1444 # 500015a4 <translation_table>
5000084e:	9736                	add	a4,a4,a3
50000850:	4314                	lw	a3,0(a4)
50000852:	02868663          	beq	a3,s0,5000087e <readfail+0x5e>
50000856:	0785                	addi	a5,a5,1
50000858:	ff4797e3          	bne	a5,s4,50000846 <readfail+0x26>
5000085c:	5d549523          	sh	s5,1482(s1) # 500025ca <stopreason>
50000860:	5c8b2223          	sw	s0,1476(s6) # 500025c4 <required_object>
50000864:	50002937          	lui	s2,0x50002
50000868:	792000ef          	jal	50000ffa <ulp_lp_core_wakeup_main_processor>
5000086c:	5c895783          	lhu	a5,1480(s2) # 500025c8 <ack>
50000870:	dfe5                	beqz	a5,50000868 <readfail+0x48>
50000872:	5c091423          	sh	zero,1480(s2)
50000876:	47a9                	li	a5,10
50000878:	5cf49523          	sh	a5,1482(s1)
5000087c:	b7e1                	j	50000844 <readfail+0x24>
5000087e:	40f2                	lw	ra,28(sp)
50000880:	4462                	lw	s0,24(sp)
50000882:	4348                	lw	a0,4(a4)
50000884:	44d2                	lw	s1,20(sp)
50000886:	4942                	lw	s2,16(sp)
50000888:	49b2                	lw	s3,12(sp)
5000088a:	4a22                	lw	s4,8(sp)
5000088c:	4a92                	lw	s5,4(sp)
5000088e:	4b02                	lw	s6,0(sp)
50000890:	6105                	addi	sp,sp,32
50000892:	8082                	ret

50000894 <main>:
50000894:	1141                	addi	sp,sp,-16
50000896:	500027b7          	lui	a5,0x50002
5000089a:	4729                	li	a4,10
5000089c:	4505                	li	a0,1
5000089e:	c606                	sw	ra,12(sp)
500008a0:	5ce79523          	sh	a4,1482(a5) # 500025ca <stopreason>
500008a4:	7c8000ef          	jal	5000106c <ulp_lp_core_lp_timer_intr_enable>
500008a8:	500017b7          	lui	a5,0x50001
500008ac:	5987a783          	lw	a5,1432(a5) # 50001598 <entrypoint>
500008b0:	9782                	jalr	a5
500008b2:	40b2                	lw	ra,12(sp)
500008b4:	4501                	li	a0,0
500008b6:	0141                	addi	sp,sp,16
500008b8:	8082                	ret

500008ba <mrbc_gc_sweep>:
500008ba:	500016b7          	lui	a3,0x50001
500008be:	57068693          	addi	a3,a3,1392 # 50001570 <heaps>
500008c2:	01868593          	addi	a1,a3,24
500008c6:	450d                	li	a0,3
500008c8:	42dc                	lw	a5,4(a3)
500008ca:	4298                	lw	a4,0(a3)
500008cc:	8f1d                	sub	a4,a4,a5
500008ce:	02a75733          	divu	a4,a4,a0
500008d2:	00e78633          	add	a2,a5,a4
500008d6:	9732                	add	a4,a4,a2
500008d8:	8f1d                	sub	a4,a4,a5
500008da:	00f70833          	add	a6,a4,a5
500008de:	06c7e863          	bltu	a5,a2,5000094e <mrbc_gc_sweep+0x94>
500008e2:	06a1                	addi	a3,a3,8
500008e4:	fed592e3          	bne	a1,a3,500008c8 <mrbc_gc_sweep+0xe>
500008e8:	500027b7          	lui	a5,0x50002
500008ec:	4581                	li	a1,0
500008ee:	4501                	li	a0,0
500008f0:	00078793          	mv	a5,a5
500008f4:	04000893          	li	a7,64
500008f8:	02000313          	li	t1,32
500008fc:	4794                	lw	a3,8(a5)
500008fe:	4398                	lw	a4,0(a5)
50000900:	0187a803          	lw	a6,24(a5) # 50002018 <mrbc_gc_space+0xa54>
50000904:	40b88e33          	sub	t3,a7,a1
50000908:	8f55                	or	a4,a4,a3
5000090a:	01077733          	and	a4,a4,a6
5000090e:	4681                	li	a3,0
50000910:	4601                	li	a2,0
50000912:	00187e93          	andi	t4,a6,1
50000916:	8205                	srli	a2,a2,0x1
50000918:	000e8563          	beqz	t4,50000922 <mrbc_gc_sweep+0x68>
5000091c:	00d75533          	srl	a0,a4,a3
50000920:	8905                	andi	a0,a0,1
50000922:	01f51e93          	slli	t4,a0,0x1f
50000926:	0685                	addi	a3,a3,1
50000928:	00cee633          	or	a2,t4,a2
5000092c:	00185813          	srli	a6,a6,0x1
50000930:	00668463          	beq	a3,t1,50000938 <mrbc_gc_sweep+0x7e>
50000934:	fdc69fe3          	bne	a3,t3,50000912 <mrbc_gc_sweep+0x58>
50000938:	95b6                	add	a1,a1,a3
5000093a:	4b94                	lw	a3,16(a5)
5000093c:	cf98                	sw	a4,24(a5)
5000093e:	0007a023          	sw	zero,0(a5)
50000942:	8ef1                	and	a3,a3,a2
50000944:	cb94                	sw	a3,16(a5)
50000946:	0791                	addi	a5,a5,4
50000948:	fb159ae3          	bne	a1,a7,500008fc <mrbc_gc_sweep+0x42>
5000094c:	8082                	ret
5000094e:	0007a883          	lw	a7,0(a5)
50000952:	01182023          	sw	a7,0(a6)
50000956:	0007a023          	sw	zero,0(a5)
5000095a:	0791                	addi	a5,a5,4
5000095c:	bfbd                	j	500008da <mrbc_gc_sweep+0x20>

5000095e <mrbc_gc_mark>:
5000095e:	00157693          	andi	a3,a0,1
50000962:	e6e5                	bnez	a3,50000a4a <mrbc_gc_mark+0xec>
50000964:	1141                	addi	sp,sp,-16
50000966:	500017b7          	lui	a5,0x50001
5000096a:	c422                	sw	s0,8(sp)
5000096c:	c606                	sw	ra,12(sp)
5000096e:	c226                	sw	s1,4(sp)
50000970:	c04a                	sw	s2,0(sp)
50000972:	842a                	mv	s0,a0
50000974:	57078613          	addi	a2,a5,1392 # 50001570 <heaps>
50000978:	57078713          	addi	a4,a5,1392
5000097c:	421c                	lw	a5,0(a2)
5000097e:	464c                	lw	a1,12(a2)
50000980:	00f46963          	bltu	s0,a5,50000992 <mrbc_gc_mark+0x34>
50000984:	00b46d63          	bltu	s0,a1,5000099e <mrbc_gc_mark+0x40>
50000988:	0685                	addi	a3,a3,1
5000098a:	4791                	li	a5,4
5000098c:	0621                	addi	a2,a2,8
5000098e:	fef697e3          	bne	a3,a5,5000097c <mrbc_gc_mark+0x1e>
50000992:	40b2                	lw	ra,12(sp)
50000994:	4422                	lw	s0,8(sp)
50000996:	4492                	lw	s1,4(sp)
50000998:	4902                	lw	s2,0(sp)
5000099a:	0141                	addi	sp,sp,16
5000099c:	8082                	ret
5000099e:	460d                	li	a2,3
500009a0:	40f40533          	sub	a0,s0,a5
500009a4:	4595                	li	a1,5
500009a6:	00c68463          	beq	a3,a2,500009ae <mrbc_gc_mark+0x50>
500009aa:	00c685b3          	add	a1,a3,a2
500009ae:	00b55633          	srl	a2,a0,a1
500009b2:	00b615b3          	sll	a1,a2,a1
500009b6:	fcb51ee3          	bne	a0,a1,50000992 <mrbc_gc_mark+0x34>
500009ba:	00369593          	slli	a1,a3,0x3
500009be:	972e                	add	a4,a4,a1
500009c0:	434c                	lw	a1,4(a4)
500009c2:	ffd68713          	addi	a4,a3,-3
500009c6:	488d                	li	a7,3
500009c8:	00173713          	seqz	a4,a4
500009cc:	8f8d                	sub	a5,a5,a1
500009ce:	9746                	add	a4,a4,a7
500009d0:	02e7d733          	divu	a4,a5,a4
500009d4:	0642                	slli	a2,a2,0x10
500009d6:	8241                	srli	a2,a2,0x10
500009d8:	00565813          	srli	a6,a2,0x5
500009dc:	00281513          	slli	a0,a6,0x2
500009e0:	0706                	slli	a4,a4,0x1
500009e2:	972e                	add	a4,a4,a1
500009e4:	972a                	add	a4,a4,a0
500009e6:	4318                	lw	a4,0(a4)
500009e8:	01169963          	bne	a3,a7,500009fa <mrbc_gc_mark+0x9c>
500009ec:	8389                	srli	a5,a5,0x2
500009ee:	02d787b3          	mul	a5,a5,a3
500009f2:	97ae                	add	a5,a5,a1
500009f4:	97aa                	add	a5,a5,a0
500009f6:	439c                	lw	a5,0(a5)
500009f8:	8f7d                	and	a4,a4,a5
500009fa:	8a7d                	andi	a2,a2,31
500009fc:	00c75733          	srl	a4,a4,a2
50000a00:	8b05                	andi	a4,a4,1
50000a02:	db41                	beqz	a4,50000992 <mrbc_gc_mark+0x34>
50000a04:	d5d9                	beqz	a1,50000992 <mrbc_gc_mark+0x34>
50000a06:	00281793          	slli	a5,a6,0x2
50000a0a:	95be                	add	a1,a1,a5
50000a0c:	419c                	lw	a5,0(a1)
50000a0e:	4705                	li	a4,1
50000a10:	00c71733          	sll	a4,a4,a2
50000a14:	8fd9                	or	a5,a5,a4
50000a16:	00045703          	lhu	a4,0(s0)
50000a1a:	c19c                	sw	a5,0(a1)
50000a1c:	47c1                	li	a5,16
50000a1e:	00e7e963          	bltu	a5,a4,50000a30 <mrbc_gc_mark+0xd2>
50000a22:	67c5                	lui	a5,0x11
50000a24:	05078793          	addi	a5,a5,80 # 11050 <RvExcFrameSize+0x10fbc>
50000a28:	00e7d7b3          	srl	a5,a5,a4
50000a2c:	8b85                	andi	a5,a5,1
50000a2e:	f3b5                	bnez	a5,50000992 <mrbc_gc_mark+0x34>
50000a30:	00440913          	addi	s2,s0,4
50000a34:	4481                	li	s1,0
50000a36:	00245783          	lhu	a5,2(s0)
50000a3a:	f4f4dce3          	bge	s1,a5,50000992 <mrbc_gc_mark+0x34>
50000a3e:	00092503          	lw	a0,0(s2)
50000a42:	0485                	addi	s1,s1,1
50000a44:	0911                	addi	s2,s2,4
50000a46:	3f21                	jal	5000095e <mrbc_gc_mark>
50000a48:	b7fd                	j	50000a36 <mrbc_gc_mark+0xd8>
50000a4a:	8082                	ret

50000a4c <mrbc_gc_mark_frozen_objects>:
50000a4c:	715d                	addi	sp,sp,-80
50000a4e:	dc52                	sw	s4,56(sp)
50000a50:	50001a37          	lui	s4,0x50001
50000a54:	de4e                	sw	s3,60(sp)
50000a56:	d462                	sw	s8,40(sp)
50000a58:	d266                	sw	s9,36(sp)
50000a5a:	d06a                	sw	s10,32(sp)
50000a5c:	ce6e                	sw	s11,28(sp)
50000a5e:	c686                	sw	ra,76(sp)
50000a60:	c4a2                	sw	s0,72(sp)
50000a62:	c2a6                	sw	s1,68(sp)
50000a64:	c0ca                	sw	s2,64(sp)
50000a66:	da56                	sw	s5,52(sp)
50000a68:	d85a                	sw	s6,48(sp)
50000a6a:	d65e                	sw	s7,44(sp)
50000a6c:	570a0a13          	addi	s4,s4,1392 # 50001570 <heaps>
50000a70:	4981                	li	s3,0
50000a72:	4c0d                	li	s8,3
50000a74:	4c85                	li	s9,1
50000a76:	50002d37          	lui	s10,0x50002
50000a7a:	02000d93          	li	s11,32
50000a7e:	004a2483          	lw	s1,4(s4)
50000a82:	000a2403          	lw	s0,0(s4)
50000a86:	ffd98713          	addi	a4,s3,-3
50000a8a:	00173713          	seqz	a4,a4
50000a8e:	409407b3          	sub	a5,s0,s1
50000a92:	070d                	addi	a4,a4,3
50000a94:	02e7d7b3          	divu	a5,a5,a4
50000a98:	94be                	add	s1,s1,a5
50000a9a:	00f48ab3          	add	s5,s1,a5
50000a9e:	47a1                	li	a5,8
50000aa0:	01898563          	beq	s3,s8,50000aaa <mrbc_gc_mark_frozen_objects+0x5e>
50000aa4:	4789                	li	a5,2
50000aa6:	013797b3          	sll	a5,a5,s3
50000aaa:	00279713          	slli	a4,a5,0x2
50000aae:	00479913          	slli	s2,a5,0x4
50000ab2:	0354e663          	bltu	s1,s5,50000ade <mrbc_gc_mark_frozen_objects+0x92>
50000ab6:	0985                	addi	s3,s3,1
50000ab8:	4791                	li	a5,4
50000aba:	0a21                	addi	s4,s4,8
50000abc:	fcf991e3          	bne	s3,a5,50000a7e <mrbc_gc_mark_frozen_objects+0x32>
50000ac0:	40b6                	lw	ra,76(sp)
50000ac2:	4426                	lw	s0,72(sp)
50000ac4:	4496                	lw	s1,68(sp)
50000ac6:	4906                	lw	s2,64(sp)
50000ac8:	59f2                	lw	s3,60(sp)
50000aca:	5a62                	lw	s4,56(sp)
50000acc:	5ad2                	lw	s5,52(sp)
50000ace:	5b42                	lw	s6,48(sp)
50000ad0:	5bb2                	lw	s7,44(sp)
50000ad2:	5c22                	lw	s8,40(sp)
50000ad4:	5c92                	lw	s9,36(sp)
50000ad6:	5d02                	lw	s10,32(sp)
50000ad8:	4df2                	lw	s11,28(sp)
50000ada:	6161                	addi	sp,sp,80
50000adc:	8082                	ret
50000ade:	0004ab83          	lw	s7,0(s1)
50000ae2:	000b9563          	bnez	s7,50000aec <mrbc_gc_mark_frozen_objects+0xa0>
50000ae6:	944a                	add	s0,s0,s2
50000ae8:	0491                	addi	s1,s1,4
50000aea:	b7e1                	j	50000ab2 <mrbc_gc_mark_frozen_objects+0x66>
50000aec:	4b01                	li	s6,0
50000aee:	016c97b3          	sll	a5,s9,s6
50000af2:	0177f7b3          	and	a5,a5,s7
50000af6:	cb89                	beqz	a5,50000b08 <mrbc_gc_mark_frozen_objects+0xbc>
50000af8:	c63a                	sw	a4,12(sp)
50000afa:	5c4d0793          	addi	a5,s10,1476 # 500025c4 <required_object>
50000afe:	fcf471e3          	bgeu	s0,a5,50000ac0 <mrbc_gc_mark_frozen_objects+0x74>
50000b02:	8522                	mv	a0,s0
50000b04:	3da9                	jal	5000095e <mrbc_gc_mark>
50000b06:	4732                	lw	a4,12(sp)
50000b08:	0b05                	addi	s6,s6,1
50000b0a:	943a                	add	s0,s0,a4
50000b0c:	ffbb11e3          	bne	s6,s11,50000aee <mrbc_gc_mark_frozen_objects+0xa2>
50000b10:	bfe1                	j	50000ae8 <mrbc_gc_mark_frozen_objects+0x9c>

50000b12 <mrbc_gc_gc>:
50000b12:	1141                	addi	sp,sp,-16
50000b14:	c606                	sw	ra,12(sp)
50000b16:	c422                	sw	s0,8(sp)
50000b18:	c226                	sw	s1,4(sp)
50000b1a:	840a                	mv	s0,sp
50000b1c:	500034b7          	lui	s1,0x50003
50000b20:	6a048793          	addi	a5,s1,1696 # 500036a0 <s_shared_mem>
50000b24:	00f46863          	bltu	s0,a5,50000b34 <mrbc_gc_gc+0x22>
50000b28:	3715                	jal	50000a4c <mrbc_gc_mark_frozen_objects>
50000b2a:	4422                	lw	s0,8(sp)
50000b2c:	40b2                	lw	ra,12(sp)
50000b2e:	4492                	lw	s1,4(sp)
50000b30:	0141                	addi	sp,sp,16
50000b32:	b361                	j	500008ba <mrbc_gc_sweep>
50000b34:	4008                	lw	a0,0(s0)
50000b36:	0411                	addi	s0,s0,4
50000b38:	351d                	jal	5000095e <mrbc_gc_mark>
50000b3a:	b7dd                	j	50000b20 <mrbc_gc_gc+0xe>

50000b3c <mrbc_gc_alloc>:
50000b3c:	1141                	addi	sp,sp,-16
50000b3e:	c422                	sw	s0,8(sp)
50000b40:	c226                	sw	s1,4(sp)
50000b42:	842a                	mv	s0,a0
50000b44:	c606                	sw	ra,12(sp)
50000b46:	c04a                	sw	s2,0(sp)
50000b48:	4509                	li	a0,2
50000b4a:	02000493          	li	s1,32
50000b4e:	0284ed63          	bltu	s1,s0,50000b88 <mrbc_gc_alloc+0x4c>
50000b52:	50001637          	lui	a2,0x50001
50000b56:	57060613          	addi	a2,a2,1392 # 50001570 <heaps>
50000b5a:	01860e13          	addi	t3,a2,24
50000b5e:	4721                	li	a4,8
50000b60:	02876063          	bltu	a4,s0,50000b80 <mrbc_gc_alloc+0x44>
50000b64:	425c                	lw	a5,4(a2)
50000b66:	00062803          	lw	a6,0(a2)
50000b6a:	458d                	li	a1,3
50000b6c:	4881                	li	a7,0
50000b6e:	40f806b3          	sub	a3,a6,a5
50000b72:	02b6d6b3          	divu	a3,a3,a1
50000b76:	5efd                	li	t4,-1
50000b78:	0686                	slli	a3,a3,0x1
50000b7a:	96be                	add	a3,a3,a5
50000b7c:	0506ea63          	bltu	a3,a6,50000bd0 <mrbc_gc_alloc+0x94>
50000b80:	0621                	addi	a2,a2,8
50000b82:	0706                	slli	a4,a4,0x1
50000b84:	fdc61ee3          	bne	a2,t3,50000b60 <mrbc_gc_alloc+0x24>
50000b88:	01f40713          	addi	a4,s0,31
50000b8c:	8315                	srli	a4,a4,0x5
50000b8e:	4781                	li	a5,0
50000b90:	500018b7          	lui	a7,0x50001
50000b94:	4e85                	li	t4,1
50000b96:	02d00e13          	li	t3,45
50000b9a:	0057d613          	srli	a2,a5,0x5
50000b9e:	29360593          	addi	a1,a2,659
50000ba2:	5c488693          	addi	a3,a7,1476 # 500015c4 <mrbc_gc_space>
50000ba6:	058a                	slli	a1,a1,0x2
50000ba8:	96ae                	add	a3,a3,a1
50000baa:	4294                	lw	a3,0(a3)
50000bac:	01f7f313          	andi	t1,a5,31
50000bb0:	5c488813          	addi	a6,a7,1476
50000bb4:	0066d6b3          	srl	a3,a3,t1
50000bb8:	8a85                	andi	a3,a3,1
50000bba:	c6bd                	beqz	a3,50000c28 <mrbc_gc_alloc+0xec>
50000bbc:	00178693          	addi	a3,a5,1
50000bc0:	0fc69063          	bne	a3,t3,50000ca0 <mrbc_gc_alloc+0x164>
50000bc4:	4905                	li	s2,1
50000bc6:	0f250363          	beq	a0,s2,50000cac <mrbc_gc_alloc+0x170>
50000bca:	37a1                	jal	50000b12 <mrbc_gc_gc>
50000bcc:	854a                	mv	a0,s2
50000bce:	b741                	j	50000b4e <mrbc_gc_alloc+0x12>
50000bd0:	428c                	lw	a1,0(a3)
50000bd2:	03d59563          	bne	a1,t4,50000bfc <mrbc_gc_alloc+0xc0>
50000bd6:	02088893          	addi	a7,a7,32
50000bda:	0691                	addi	a3,a3,4
50000bdc:	b745                	j	50000b7c <mrbc_gc_alloc+0x40>
50000bde:	00ff9333          	sll	t1,t6,a5
50000be2:	00b372b3          	and	t0,t1,a1
50000be6:	00028f63          	beqz	t0,50000c04 <mrbc_gc_alloc+0xc8>
50000bea:	0785                	addi	a5,a5,1
50000bec:	ffe799e3          	bne	a5,t5,50000bde <mrbc_gc_alloc+0xa2>
50000bf0:	b7dd                	j	50000bd6 <mrbc_gc_alloc+0x9a>
50000bf2:	4781                	li	a5,0
50000bf4:	02000f13          	li	t5,32
50000bf8:	4f85                	li	t6,1
50000bfa:	bfc5                	j	50000bea <mrbc_gc_alloc+0xae>
50000bfc:	0015f793          	andi	a5,a1,1
50000c00:	fbed                	bnez	a5,50000bf2 <mrbc_gc_alloc+0xb6>
50000c02:	4305                	li	t1,1
50000c04:	01178533          	add	a0,a5,a7
50000c08:	02e50533          	mul	a0,a0,a4
50000c0c:	0065e5b3          	or	a1,a1,t1
50000c10:	c28c                	sw	a1,0(a3)
50000c12:	9542                	add	a0,a0,a6
50000c14:	972a                	add	a4,a4,a0
50000c16:	87aa                	mv	a5,a0
50000c18:	08e7e663          	bltu	a5,a4,50000ca4 <mrbc_gc_alloc+0x168>
50000c1c:	40b2                	lw	ra,12(sp)
50000c1e:	4422                	lw	s0,8(sp)
50000c20:	4492                	lw	s1,4(sp)
50000c22:	4902                	lw	s2,0(sp)
50000c24:	0141                	addi	sp,sp,16
50000c26:	8082                	ret
50000c28:	86be                	mv	a3,a5
50000c2a:	40fe8f33          	sub	t5,t4,a5
50000c2e:	0056d593          	srli	a1,a3,0x5
50000c32:	29358593          	addi	a1,a1,659
50000c36:	058a                	slli	a1,a1,0x2
50000c38:	95c2                	add	a1,a1,a6
50000c3a:	418c                	lw	a1,0(a1)
50000c3c:	00d5d5b3          	srl	a1,a1,a3
50000c40:	8985                	andi	a1,a1,1
50000c42:	edb9                	bnez	a1,50000ca0 <mrbc_gc_alloc+0x164>
50000c44:	00df05b3          	add	a1,t5,a3
50000c48:	04e5e863          	bltu	a1,a4,50000c98 <mrbc_gc_alloc+0x15c>
50000c4c:	853e                	mv	a0,a5
50000c4e:	4e05                	li	t3,1
50000c50:	a005                	j	50000c70 <mrbc_gc_alloc+0x134>
50000c52:	00555593          	srli	a1,a0,0x5
50000c56:	29358593          	addi	a1,a1,659
50000c5a:	058a                	slli	a1,a1,0x2
50000c5c:	95c2                	add	a1,a1,a6
50000c5e:	0005ae83          	lw	t4,0(a1)
50000c62:	00ae18b3          	sll	a7,t3,a0
50000c66:	0505                	addi	a0,a0,1
50000c68:	01d8e8b3          	or	a7,a7,t4
50000c6c:	0115a023          	sw	a7,0(a1)
50000c70:	fea6f1e3          	bgeu	a3,a0,50000c52 <mrbc_gc_alloc+0x116>
50000c74:	29560613          	addi	a2,a2,661
50000c78:	060a                	slli	a2,a2,0x2
50000c7a:	9642                	add	a2,a2,a6
50000c7c:	4214                	lw	a3,0(a2)
50000c7e:	4585                	li	a1,1
50000c80:	006595b3          	sll	a1,a1,t1
50000c84:	8ecd                	or	a3,a3,a1
50000c86:	c214                	sw	a3,0(a2)
50000c88:	500026b7          	lui	a3,0x50002
50000c8c:	0796                	slli	a5,a5,0x5
50000c8e:	02068693          	addi	a3,a3,32 # 50002020 <mrbc_gc_space+0xa5c>
50000c92:	00d78533          	add	a0,a5,a3
50000c96:	bfbd                	j	50000c14 <mrbc_gc_alloc+0xd8>
50000c98:	0685                	addi	a3,a3,1
50000c9a:	f9c69ae3          	bne	a3,t3,50000c2e <mrbc_gc_alloc+0xf2>
50000c9e:	b71d                	j	50000bc4 <mrbc_gc_alloc+0x88>
50000ca0:	87b6                	mv	a5,a3
50000ca2:	bde5                	j	50000b9a <mrbc_gc_alloc+0x5e>
50000ca4:	0007a023          	sw	zero,0(a5)
50000ca8:	0791                	addi	a5,a5,4
50000caa:	b7bd                	j	50000c18 <mrbc_gc_alloc+0xdc>
50000cac:	4501                	li	a0,0
50000cae:	b7bd                	j	50000c1c <mrbc_gc_alloc+0xe0>

50000cb0 <_panic_handler>:
50000cb0:	7135                	addi	sp,sp,-160
50000cb2:	c206                	sw	ra,4(sp)
50000cb4:	c812                	sw	tp,16(sp)
50000cb6:	ca16                	sw	t0,20(sp)
50000cb8:	cc1a                	sw	t1,24(sp)
50000cba:	ce1e                	sw	t2,28(sp)
50000cbc:	d022                	sw	s0,32(sp)
50000cbe:	d226                	sw	s1,36(sp)
50000cc0:	d42a                	sw	a0,40(sp)
50000cc2:	d62e                	sw	a1,44(sp)
50000cc4:	d832                	sw	a2,48(sp)
50000cc6:	da36                	sw	a3,52(sp)
50000cc8:	dc3a                	sw	a4,56(sp)
50000cca:	de3e                	sw	a5,60(sp)
50000ccc:	c0c2                	sw	a6,64(sp)
50000cce:	c2c6                	sw	a7,68(sp)
50000cd0:	c4ca                	sw	s2,72(sp)
50000cd2:	c6ce                	sw	s3,76(sp)
50000cd4:	c8d2                	sw	s4,80(sp)
50000cd6:	cad6                	sw	s5,84(sp)
50000cd8:	ccda                	sw	s6,88(sp)
50000cda:	cede                	sw	s7,92(sp)
50000cdc:	d0e2                	sw	s8,96(sp)
50000cde:	d2e6                	sw	s9,100(sp)
50000ce0:	d4ea                	sw	s10,104(sp)
50000ce2:	d6ee                	sw	s11,108(sp)
50000ce4:	d8f2                	sw	t3,112(sp)
50000ce6:	daf6                	sw	t4,116(sp)
50000ce8:	dcfa                	sw	t5,120(sp)
50000cea:	defe                	sw	t6,124(sp)
50000cec:	341022f3          	csrr	t0,mepc
50000cf0:	c016                	sw	t0,0(sp)
50000cf2:	0a010293          	addi	t0,sp,160
50000cf6:	c416                	sw	t0,8(sp)
50000cf8:	300022f3          	csrr	t0,mstatus
50000cfc:	c116                	sw	t0,128(sp)
50000cfe:	342022f3          	csrr	t0,mcause
50000d02:	c516                	sw	t0,136(sp)
50000d04:	305022f3          	csrr	t0,mtvec
50000d08:	c316                	sw	t0,132(sp)
50000d0a:	f14022f3          	csrr	t0,mhartid
50000d0e:	c916                	sw	t0,144(sp)
50000d10:	343022f3          	csrr	t0,mtval
50000d14:	c716                	sw	t0,140(sp)
50000d16:	342025f3          	csrr	a1,mcause
50000d1a:	850a                	mv	a0,sp
50000d1c:	2641                	jal	5000109c <ulp_lp_core_panic_handler>

50000d1e <_end>:
50000d1e:	a001                	j	50000d1e <_end>

50000d20 <_interrupt_handler>:
50000d20:	7119                	addi	sp,sp,-128
50000d22:	c206                	sw	ra,4(sp)
50000d24:	c812                	sw	tp,16(sp)
50000d26:	ca16                	sw	t0,20(sp)
50000d28:	cc1a                	sw	t1,24(sp)
50000d2a:	ce1e                	sw	t2,28(sp)
50000d2c:	d022                	sw	s0,32(sp)
50000d2e:	d226                	sw	s1,36(sp)
50000d30:	d42a                	sw	a0,40(sp)
50000d32:	d62e                	sw	a1,44(sp)
50000d34:	d832                	sw	a2,48(sp)
50000d36:	da36                	sw	a3,52(sp)
50000d38:	dc3a                	sw	a4,56(sp)
50000d3a:	de3e                	sw	a5,60(sp)
50000d3c:	c0c2                	sw	a6,64(sp)
50000d3e:	c2c6                	sw	a7,68(sp)
50000d40:	c4ca                	sw	s2,72(sp)
50000d42:	c6ce                	sw	s3,76(sp)
50000d44:	c8d2                	sw	s4,80(sp)
50000d46:	cad6                	sw	s5,84(sp)
50000d48:	ccda                	sw	s6,88(sp)
50000d4a:	cede                	sw	s7,92(sp)
50000d4c:	d0e2                	sw	s8,96(sp)
50000d4e:	d2e6                	sw	s9,100(sp)
50000d50:	d4ea                	sw	s10,104(sp)
50000d52:	d6ee                	sw	s11,108(sp)
50000d54:	d8f2                	sw	t3,112(sp)
50000d56:	daf6                	sw	t4,116(sp)
50000d58:	dcfa                	sw	t5,120(sp)
50000d5a:	defe                	sw	t6,124(sp)
50000d5c:	341022f3          	csrr	t0,mepc
50000d60:	c016                	sw	t0,0(sp)
50000d62:	2681                	jal	500010a2 <ulp_lp_core_intr_handler>
50000d64:	4282                	lw	t0,0(sp)
50000d66:	34129073          	csrw	mepc,t0
50000d6a:	4092                	lw	ra,4(sp)
50000d6c:	4242                	lw	tp,16(sp)
50000d6e:	42d2                	lw	t0,20(sp)
50000d70:	4362                	lw	t1,24(sp)
50000d72:	43f2                	lw	t2,28(sp)
50000d74:	5402                	lw	s0,32(sp)
50000d76:	5492                	lw	s1,36(sp)
50000d78:	5522                	lw	a0,40(sp)
50000d7a:	55b2                	lw	a1,44(sp)
50000d7c:	5642                	lw	a2,48(sp)
50000d7e:	56d2                	lw	a3,52(sp)
50000d80:	5762                	lw	a4,56(sp)
50000d82:	57f2                	lw	a5,60(sp)
50000d84:	4806                	lw	a6,64(sp)
50000d86:	4896                	lw	a7,68(sp)
50000d88:	4926                	lw	s2,72(sp)
50000d8a:	49b6                	lw	s3,76(sp)
50000d8c:	4a46                	lw	s4,80(sp)
50000d8e:	4ad6                	lw	s5,84(sp)
50000d90:	4b66                	lw	s6,88(sp)
50000d92:	4bf6                	lw	s7,92(sp)
50000d94:	5c06                	lw	s8,96(sp)
50000d96:	5c96                	lw	s9,100(sp)
50000d98:	5d26                	lw	s10,104(sp)
50000d9a:	5db6                	lw	s11,108(sp)
50000d9c:	5e46                	lw	t3,112(sp)
50000d9e:	5ed6                	lw	t4,116(sp)
50000da0:	5f66                	lw	t5,120(sp)
50000da2:	5ff6                	lw	t6,124(sp)
50000da4:	6109                	addi	sp,sp,128
50000da6:	30200073          	mret

50000daa <ulp_lp_core_memory_shared_cfg_get>:
50000daa:	50003537          	lui	a0,0x50003
50000dae:	6a050513          	addi	a0,a0,1696 # 500036a0 <s_shared_mem>
50000db2:	8082                	ret

50000db4 <lp_timer_hal_set_alarm_target>:
50000db4:	600b17b7          	lui	a5,0x600b1
50000db8:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
50000dbc:	43f8                	lw	a4,68(a5)
50000dbe:	800006b7          	lui	a3,0x80000
50000dc2:	1141                	addi	sp,sp,-16
50000dc4:	8f55                	or	a4,a4,a3
50000dc6:	c3f8                	sw	a4,68(a5)
50000dc8:	47d0                	lw	a2,12(a5)
50000dca:	05c2                	slli	a1,a1,0x10
50000dcc:	81c1                	srli	a1,a1,0x10
50000dce:	c432                	sw	a2,8(sp)
50000dd0:	00b11423          	sh	a1,8(sp)
50000dd4:	4622                	lw	a2,8(sp)
50000dd6:	c7d0                	sw	a2,12(a5)
50000dd8:	4790                	lw	a2,8(a5)
50000dda:	c632                	sw	a2,12(sp)
50000ddc:	c62a                	sw	a0,12(sp)
50000dde:	4732                	lw	a4,12(sp)
50000de0:	c798                	sw	a4,8(a5)
50000de2:	47d8                	lw	a4,12(a5)
50000de4:	8f55                	or	a4,a4,a3
50000de6:	c7d8                	sw	a4,12(a5)
50000de8:	0141                	addi	sp,sp,16
50000dea:	8082                	ret

50000dec <ulp_lp_core_lp_timer_get_cycle_count>:
50000dec:	600b17b7          	lui	a5,0x600b1
50000df0:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
50000df4:	4b98                	lw	a4,16(a5)
50000df6:	100006b7          	lui	a3,0x10000
50000dfa:	1101                	addi	sp,sp,-32
50000dfc:	8f55                	or	a4,a4,a3
50000dfe:	cb98                	sw	a4,16(a5)
50000e00:	4bd8                	lw	a4,20(a5)
50000e02:	ce3a                	sw	a4,28(sp)
50000e04:	4572                	lw	a0,28(sp)
50000e06:	4f9c                	lw	a5,24(a5)
50000e08:	cc3e                	sw	a5,24(sp)
50000e0a:	01815583          	lhu	a1,24(sp)
50000e0e:	6105                	addi	sp,sp,32
50000e10:	8082                	ret

50000e12 <ulp_lp_core_lp_timer_set_wakeup_time>:
50000e12:	1101                	addi	sp,sp,-32
50000e14:	ce06                	sw	ra,28(sp)
50000e16:	cc22                	sw	s0,24(sp)
50000e18:	ca26                	sw	s1,20(sp)
50000e1a:	c84a                	sw	s2,16(sp)
50000e1c:	c64e                	sw	s3,12(sp)
50000e1e:	892a                	mv	s2,a0
50000e20:	84ae                	mv	s1,a1
50000e22:	37e9                	jal	50000dec <ulp_lp_core_lp_timer_get_cycle_count>
50000e24:	600b17b7          	lui	a5,0x600b1
50000e28:	43d0                	lw	a2,4(a5)
50000e2a:	89ae                	mv	s3,a1
50000e2c:	04ce                	slli	s1,s1,0x13
50000e2e:	00d95593          	srli	a1,s2,0xd
50000e32:	842a                	mv	s0,a0
50000e34:	8dc5                	or	a1,a1,s1
50000e36:	01391513          	slli	a0,s2,0x13
50000e3a:	4681                	li	a3,0
50000e3c:	beeff0ef          	jal	5000022a <__udivdi3>
50000e40:	87aa                	mv	a5,a0
50000e42:	9522                	add	a0,a0,s0
50000e44:	4462                	lw	s0,24(sp)
50000e46:	40f2                	lw	ra,28(sp)
50000e48:	44d2                	lw	s1,20(sp)
50000e4a:	4942                	lw	s2,16(sp)
50000e4c:	95ce                	add	a1,a1,s3
50000e4e:	49b2                	lw	s3,12(sp)
50000e50:	00f537b3          	sltu	a5,a0,a5
50000e54:	95be                	add	a1,a1,a5
50000e56:	6105                	addi	sp,sp,32
50000e58:	bfb1                	j	50000db4 <lp_timer_hal_set_alarm_target>

50000e5a <ulp_lp_core_lp_timer_set_wakeup_ticks>:
50000e5a:	1141                	addi	sp,sp,-16
50000e5c:	c422                	sw	s0,8(sp)
50000e5e:	c226                	sw	s1,4(sp)
50000e60:	c606                	sw	ra,12(sp)
50000e62:	842a                	mv	s0,a0
50000e64:	84ae                	mv	s1,a1
50000e66:	3759                	jal	50000dec <ulp_lp_core_lp_timer_get_cycle_count>
50000e68:	87aa                	mv	a5,a0
50000e6a:	9522                	add	a0,a0,s0
50000e6c:	4422                	lw	s0,8(sp)
50000e6e:	40b2                	lw	ra,12(sp)
50000e70:	95a6                	add	a1,a1,s1
50000e72:	4492                	lw	s1,4(sp)
50000e74:	00f537b3          	sltu	a5,a0,a5
50000e78:	95be                	add	a1,a1,a5
50000e7a:	0141                	addi	sp,sp,16
50000e7c:	bf25                	j	50000db4 <lp_timer_hal_set_alarm_target>

50000e7e <ulp_lp_core_lp_timer_disable>:
50000e7e:	600b17b7          	lui	a5,0x600b1
50000e82:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
50000e86:	47d8                	lw	a4,12(a5)
50000e88:	800006b7          	lui	a3,0x80000
50000e8c:	fff68613          	addi	a2,a3,-1 # 7fffffff <LPPERI+0x1ff4d7ff>
50000e90:	8f71                	and	a4,a4,a2
50000e92:	c7d8                	sw	a4,12(a5)
50000e94:	43f8                	lw	a4,68(a5)
50000e96:	8f55                	or	a4,a4,a3
50000e98:	c3f8                	sw	a4,68(a5)
50000e9a:	8082                	ret

50000e9c <lp_core_startup>:
50000e9c:	1141                	addi	sp,sp,-16
50000e9e:	c606                	sw	ra,12(sp)
50000ea0:	2819                	jal	50000eb6 <ulp_lp_core_update_wakeup_cause>
50000ea2:	3acd                	jal	50000894 <main>
50000ea4:	3719                	jal	50000daa <ulp_lp_core_memory_shared_cfg_get>
50000ea6:	87aa                	mv	a5,a0
50000ea8:	47cc                	lw	a1,12(a5)
50000eaa:	4508                	lw	a0,8(a0)
50000eac:	00b567b3          	or	a5,a0,a1
50000eb0:	c391                	beqz	a5,50000eb4 <lp_core_startup+0x18>
50000eb2:	3765                	jal	50000e5a <ulp_lp_core_lp_timer_set_wakeup_ticks>
50000eb4:	2ab5                	jal	50001030 <ulp_lp_core_halt>

50000eb6 <ulp_lp_core_update_wakeup_cause>:
50000eb6:	600b0737          	lui	a4,0x600b0
50000eba:	00070693          	mv	a3,a4
50000ebe:	1806d683          	lhu	a3,384(a3)
50000ec2:	500027b7          	lui	a5,0x50002
50000ec6:	5c07a823          	sw	zero,1488(a5) # 500025d0 <lp_wakeup_cause>
50000eca:	8a85                	andi	a3,a3,1
50000ecc:	00070713          	mv	a4,a4
50000ed0:	ce81                	beqz	a3,50000ee8 <ulp_lp_core_update_wakeup_cause+0x32>
50000ed2:	16c72683          	lw	a3,364(a4) # 600b016c <PMU+0x16c>
50000ed6:	0006d963          	bgez	a3,50000ee8 <ulp_lp_core_update_wakeup_cause+0x32>
50000eda:	4685                	li	a3,1
50000edc:	5cd7a823          	sw	a3,1488(a5)
50000ee0:	800006b7          	lui	a3,0x80000
50000ee4:	16d72c23          	sw	a3,376(a4)
50000ee8:	18075683          	lhu	a3,384(a4)
50000eec:	8a89                	andi	a3,a3,2
50000eee:	c29d                	beqz	a3,50000f14 <ulp_lp_core_update_wakeup_cause+0x5e>
50000ef0:	600b16b7          	lui	a3,0x600b1
50000ef4:	40068693          	addi	a3,a3,1024 # 600b1400 <LP_UART>
50000ef8:	42d0                	lw	a2,4(a3)
50000efa:	00c61593          	slli	a1,a2,0xc
50000efe:	0005db63          	bgez	a1,50000f14 <ulp_lp_core_update_wakeup_cause+0x5e>
50000f02:	5d07a603          	lw	a2,1488(a5)
50000f06:	00266613          	ori	a2,a2,2
50000f0a:	5cc7a823          	sw	a2,1488(a5)
50000f0e:	00080637          	lui	a2,0x80
50000f12:	ca90                	sw	a2,16(a3)
50000f14:	18075683          	lhu	a3,384(a4)
50000f18:	8a91                	andi	a3,a3,4
50000f1a:	c6c1                	beqz	a3,50000fa2 <ulp_lp_core_update_wakeup_cause+0xec>
50000f1c:	600b26b7          	lui	a3,0x600b2
50000f20:	00068693          	mv	a3,a3
50000f24:	4e90                	lw	a2,24(a3)
50000f26:	1141                	addi	sp,sp,-16
50000f28:	c432                	sw	a2,8(sp)
50000f2a:	00814603          	lbu	a2,8(sp)
50000f2e:	ce11                	beqz	a2,50000f4a <ulp_lp_core_update_wakeup_cause+0x94>
50000f30:	5d07a603          	lw	a2,1488(a5)
50000f34:	00466613          	ori	a2,a2,4
50000f38:	5cc7a823          	sw	a2,1488(a5)
50000f3c:	5290                	lw	a2,32(a3)
50000f3e:	c632                	sw	a2,12(sp)
50000f40:	567d                	li	a2,-1
50000f42:	00c10623          	sb	a2,12(sp)
50000f46:	4632                	lw	a2,12(sp)
50000f48:	d290                	sw	a2,32(a3)
50000f4a:	18075683          	lhu	a3,384(a4)
50000f4e:	8aa1                	andi	a3,a3,8
50000f50:	c29d                	beqz	a3,50000f76 <ulp_lp_core_update_wakeup_cause+0xc0>
50000f52:	600b16b7          	lui	a3,0x600b1
50000f56:	00068693          	mv	a3,a3
50000f5a:	4ab0                	lw	a2,80(a3)
50000f5c:	8205                	srli	a2,a2,0x1
50000f5e:	8a05                	andi	a2,a2,1
50000f60:	ca19                	beqz	a2,50000f76 <ulp_lp_core_update_wakeup_cause+0xc0>
50000f62:	5d07a603          	lw	a2,1488(a5)
50000f66:	00866613          	ori	a2,a2,8
50000f6a:	5cc7a823          	sw	a2,1488(a5)
50000f6e:	4ab0                	lw	a2,80(a3)
50000f70:	00166613          	ori	a2,a2,1
50000f74:	cab0                	sw	a2,80(a3)
50000f76:	18075703          	lhu	a4,384(a4)
50000f7a:	8b41                	andi	a4,a4,16
50000f7c:	c30d                	beqz	a4,50000f9e <ulp_lp_core_update_wakeup_cause+0xe8>
50000f7e:	600b1737          	lui	a4,0x600b1
50000f82:	c0070713          	addi	a4,a4,-1024 # 600b0c00 <LP_TIMER>
50000f86:	5f14                	lw	a3,56(a4)
50000f88:	0006db63          	bgez	a3,50000f9e <ulp_lp_core_update_wakeup_cause+0xe8>
50000f8c:	5d07a683          	lw	a3,1488(a5)
50000f90:	0106e693          	ori	a3,a3,16
50000f94:	5cd7a823          	sw	a3,1488(a5)
50000f98:	800007b7          	lui	a5,0x80000
50000f9c:	c37c                	sw	a5,68(a4)
50000f9e:	0141                	addi	sp,sp,16
50000fa0:	8082                	ret
50000fa2:	18075683          	lhu	a3,384(a4)
50000fa6:	8aa1                	andi	a3,a3,8
50000fa8:	c29d                	beqz	a3,50000fce <ulp_lp_core_update_wakeup_cause+0x118>
50000faa:	600b16b7          	lui	a3,0x600b1
50000fae:	00068693          	mv	a3,a3
50000fb2:	4ab0                	lw	a2,80(a3)
50000fb4:	8205                	srli	a2,a2,0x1
50000fb6:	8a05                	andi	a2,a2,1
50000fb8:	ca19                	beqz	a2,50000fce <ulp_lp_core_update_wakeup_cause+0x118>
50000fba:	5d07a603          	lw	a2,1488(a5) # 800005d0 <LPPERI+0x1ff4ddd0>
50000fbe:	00866613          	ori	a2,a2,8
50000fc2:	5cc7a823          	sw	a2,1488(a5)
50000fc6:	4ab0                	lw	a2,80(a3)
50000fc8:	00166613          	ori	a2,a2,1
50000fcc:	cab0                	sw	a2,80(a3)
50000fce:	18075703          	lhu	a4,384(a4)
50000fd2:	8b41                	andi	a4,a4,16
50000fd4:	c315                	beqz	a4,50000ff8 <ulp_lp_core_update_wakeup_cause+0x142>
50000fd6:	600b1737          	lui	a4,0x600b1
50000fda:	c0070713          	addi	a4,a4,-1024 # 600b0c00 <LP_TIMER>
50000fde:	5f14                	lw	a3,56(a4)
50000fe0:	0006dc63          	bgez	a3,50000ff8 <ulp_lp_core_update_wakeup_cause+0x142>
50000fe4:	5d07a683          	lw	a3,1488(a5)
50000fe8:	0106e693          	ori	a3,a3,16
50000fec:	5cd7a823          	sw	a3,1488(a5)
50000ff0:	800007b7          	lui	a5,0x80000
50000ff4:	c37c                	sw	a5,68(a4)
50000ff6:	8082                	ret
50000ff8:	8082                	ret

50000ffa <ulp_lp_core_wakeup_main_processor>:
50000ffa:	600b07b7          	lui	a5,0x600b0
50000ffe:	18478793          	addi	a5,a5,388 # 600b0184 <PMU+0x184>
50001002:	4398                	lw	a4,0(a5)
50001004:	400006b7          	lui	a3,0x40000
50001008:	8f55                	or	a4,a4,a3
5000100a:	c398                	sw	a4,0(a5)
5000100c:	8082                	ret

5000100e <ulp_lp_core_delay_us>:
5000100e:	b0002773          	csrr	a4,mcycle
50001012:	0512                	slli	a0,a0,0x4
50001014:	b00027f3          	csrr	a5,mcycle
50001018:	8f99                	sub	a5,a5,a4
5000101a:	fea7ede3          	bltu	a5,a0,50001014 <ulp_lp_core_delay_us+0x6>
5000101e:	8082                	ret

50001020 <ulp_lp_core_delay_cycles>:
50001020:	b0002773          	csrr	a4,mcycle
50001024:	b00027f3          	csrr	a5,mcycle
50001028:	8f99                	sub	a5,a5,a4
5000102a:	fea7ede3          	bltu	a5,a0,50001024 <ulp_lp_core_delay_cycles+0x4>
5000102e:	8082                	ret

50001030 <ulp_lp_core_halt>:
50001030:	600b07b7          	lui	a5,0x600b0
50001034:	00078793          	mv	a5,a5
50001038:	1807a703          	lw	a4,384(a5) # 600b0180 <PMU+0x180>
5000103c:	800006b7          	lui	a3,0x80000
50001040:	8f55                	or	a4,a4,a3
50001042:	18e7a023          	sw	a4,384(a5)
50001046:	a001                	j	50001046 <ulp_lp_core_halt+0x16>

50001048 <ulp_lp_core_stop_lp_core>:
50001048:	600b07b7          	lui	a5,0x600b0
5000104c:	00078793          	mv	a5,a5
50001050:	18079023          	sh	zero,384(a5) # 600b0180 <PMU+0x180>
50001054:	1807a703          	lw	a4,384(a5)
50001058:	800006b7          	lui	a3,0x80000
5000105c:	8f55                	or	a4,a4,a3
5000105e:	18e7a023          	sw	a4,384(a5)
50001062:	8082                	ret

50001064 <ulp_lp_core_abort>:
50001064:	1141                	addi	sp,sp,-16
50001066:	c606                	sw	ra,12(sp)
50001068:	37c5                	jal	50001048 <ulp_lp_core_stop_lp_core>
5000106a:	a001                	j	5000106a <ulp_lp_core_abort+0x6>

5000106c <ulp_lp_core_lp_timer_intr_enable>:
5000106c:	600b1737          	lui	a4,0x600b1
50001070:	c0070713          	addi	a4,a4,-1024 # 600b0c00 <LP_TIMER>
50001074:	433c                	lw	a5,64(a4)
50001076:	057e                	slli	a0,a0,0x1f
50001078:	0786                	slli	a5,a5,0x1
5000107a:	8385                	srli	a5,a5,0x1
5000107c:	8fc9                	or	a5,a5,a0
5000107e:	c33c                	sw	a5,64(a4)
50001080:	8082                	ret

50001082 <ulp_lp_core_lp_timer_intr_clear>:
50001082:	600b17b7          	lui	a5,0x600b1
50001086:	c0078793          	addi	a5,a5,-1024 # 600b0c00 <LP_TIMER>
5000108a:	43f8                	lw	a4,68(a5)
5000108c:	800006b7          	lui	a3,0x80000
50001090:	8f55                	or	a4,a4,a3
50001092:	c3f8                	sw	a4,68(a5)
50001094:	8082                	ret

50001096 <ulp_lp_core_efuse_intr_handler>:
50001096:	1141                	addi	sp,sp,-16
50001098:	c606                	sw	ra,12(sp)
5000109a:	37e9                	jal	50001064 <ulp_lp_core_abort>

5000109c <ulp_lp_core_panic_handler>:
5000109c:	1141                	addi	sp,sp,-16
5000109e:	c606                	sw	ra,12(sp)
500010a0:	37d1                	jal	50001064 <ulp_lp_core_abort>

500010a2 <ulp_lp_core_intr_handler>:
500010a2:	600b37b7          	lui	a5,0x600b3
500010a6:	1101                	addi	sp,sp,-32
500010a8:	80078793          	addi	a5,a5,-2048 # 600b2800 <LPPERI>
500010ac:	c84a                	sw	s2,16(sp)
500010ae:	0207a903          	lw	s2,32(a5)
500010b2:	cc22                	sw	s0,24(sp)
500010b4:	50001437          	lui	s0,0x50001
500010b8:	ca26                	sw	s1,20(sp)
500010ba:	c64e                	sw	s3,12(sp)
500010bc:	ce06                	sw	ra,28(sp)
500010be:	03f97913          	andi	s2,s2,63
500010c2:	55840413          	addi	s0,s0,1368 # 50001558 <s_intr_handlers>
500010c6:	4481                	li	s1,0
500010c8:	4999                	li	s3,6
500010ca:	409957b3          	sra	a5,s2,s1
500010ce:	8b85                	andi	a5,a5,1
500010d0:	c781                	beqz	a5,500010d8 <ulp_lp_core_intr_handler+0x36>
500010d2:	401c                	lw	a5,0(s0)
500010d4:	c391                	beqz	a5,500010d8 <ulp_lp_core_intr_handler+0x36>
500010d6:	9782                	jalr	a5
500010d8:	0485                	addi	s1,s1,1
500010da:	0411                	addi	s0,s0,4
500010dc:	ff3497e3          	bne	s1,s3,500010ca <ulp_lp_core_intr_handler+0x28>
500010e0:	40f2                	lw	ra,28(sp)
500010e2:	4462                	lw	s0,24(sp)
500010e4:	44d2                	lw	s1,20(sp)
500010e6:	4942                	lw	s2,16(sp)
500010e8:	49b2                	lw	s3,12(sp)
500010ea:	6105                	addi	sp,sp,32
500010ec:	8082                	ret

500010ee <lp_core_i2c_wait_for_interrupt>:
500010ee:	1101                	addi	sp,sp,-32
500010f0:	cc22                	sw	s0,24(sp)
500010f2:	ca26                	sw	s1,20(sp)
500010f4:	c84a                	sw	s2,16(sp)
500010f6:	c64e                	sw	s3,12(sp)
500010f8:	ce06                	sw	ra,28(sp)
500010fa:	842a                	mv	s0,a0
500010fc:	84ae                	mv	s1,a1
500010fe:	4901                	li	s2,0
50001100:	500019b7          	lui	s3,0x50001
50001104:	5a09a783          	lw	a5,1440(s3) # 500015a0 <dev>
50001108:	57d8                	lw	a4,44(a5)
5000110a:	00e476b3          	and	a3,s0,a4
5000110e:	ca8d                	beqz	a3,50001140 <lp_core_i2c_wait_for_interrupt+0x52>
50001110:	40077693          	andi	a3,a4,1024
50001114:	ca99                	beqz	a3,5000112a <lp_core_i2c_wait_for_interrupt+0x3c>
50001116:	d3c0                	sw	s0,36(a5)
50001118:	10800513          	li	a0,264
5000111c:	40f2                	lw	ra,28(sp)
5000111e:	4462                	lw	s0,24(sp)
50001120:	44d2                	lw	s1,20(sp)
50001122:	4942                	lw	s2,16(sp)
50001124:	49b2                	lw	s3,12(sp)
50001126:	6105                	addi	sp,sp,32
50001128:	8082                	ret
5000112a:	08077713          	andi	a4,a4,128
5000112e:	c711                	beqz	a4,5000113a <lp_core_i2c_wait_for_interrupt+0x4c>
50001130:	5794                	lw	a3,40(a5)
50001132:	fff44713          	not	a4,s0
50001136:	8f75                	and	a4,a4,a3
50001138:	d798                	sw	a4,40(a5)
5000113a:	d3c0                	sw	s0,36(a5)
5000113c:	4501                	li	a0,0
5000113e:	bff9                	j	5000111c <lp_core_i2c_wait_for_interrupt+0x2e>
50001140:	fc04c4e3          	bltz	s1,50001108 <lp_core_i2c_wait_for_interrupt+0x1a>
50001144:	4505                	li	a0,1
50001146:	0905                	addi	s2,s2,1
50001148:	3de1                	jal	50001020 <ulp_lp_core_delay_cycles>
5000114a:	fa996de3          	bltu	s2,s1,50001104 <lp_core_i2c_wait_for_interrupt+0x16>
5000114e:	5a09a783          	lw	a5,1440(s3)
50001152:	fff44713          	not	a4,s0
50001156:	10700513          	li	a0,263
5000115a:	5794                	lw	a3,40(a5)
5000115c:	8f75                	and	a4,a4,a3
5000115e:	d798                	sw	a4,40(a5)
50001160:	d3c0                	sw	s0,36(a5)
50001162:	bf6d                	j	5000111c <lp_core_i2c_wait_for_interrupt+0x2e>

50001164 <lp_core_i2c_format_cmd.constprop.0>:
50001164:	47fd                	li	a5,31
50001166:	02a7e263          	bltu	a5,a0,5000118a <lp_core_i2c_format_cmd.constprop.0+0x26>
5000116a:	060a                	slli	a2,a2,0x2
5000116c:	500017b7          	lui	a5,0x50001
50001170:	8ed1                	or	a3,a3,a2
50001172:	058e                	slli	a1,a1,0x3
50001174:	5a07a783          	lw	a5,1440(a5) # 500015a0 <dev>
50001178:	8ecd                	or	a3,a3,a1
5000117a:	03d6f693          	andi	a3,a3,61
5000117e:	0551                	addi	a0,a0,20
50001180:	06a2                	slli	a3,a3,0x8
50001182:	050a                	slli	a0,a0,0x2
50001184:	8f55                	or	a4,a4,a3
50001186:	97aa                	add	a5,a5,a0
50001188:	c798                	sw	a4,8(a5)
5000118a:	8082                	ret

5000118c <lp_core_i2c_config_device_addr.constprop.0>:
5000118c:	500017b7          	lui	a5,0x50001
50001190:	5a07a703          	lw	a4,1440(a5) # 500015a0 <dev>
50001194:	500017b7          	lui	a5,0x50001
50001198:	59c7c783          	lbu	a5,1436(a5) # 5000159c <s_ack_check_en>
5000119c:	0506                	slli	a0,a0,0x1
5000119e:	8d4d                	or	a0,a0,a1
500011a0:	0087e793          	ori	a5,a5,8
500011a4:	0397f793          	andi	a5,a5,57
500011a8:	0ff57513          	zext.b	a0,a0
500011ac:	07a2                	slli	a5,a5,0x8
500011ae:	cf48                	sw	a0,28(a4)
500011b0:	0017e793          	ori	a5,a5,1
500011b4:	cf7c                	sw	a5,92(a4)
500011b6:	4785                	li	a5,1
500011b8:	00f60023          	sb	a5,0(a2) # 80000 <RvExcFrameSize+0x7ff6c>
500011bc:	8082                	ret

500011be <lp_core_i2c_master_read_from_device>:
500011be:	711d                	addi	sp,sp,-96
500011c0:	ce86                	sw	ra,92(sp)
500011c2:	cca2                	sw	s0,88(sp)
500011c4:	caa6                	sw	s1,84(sp)
500011c6:	c8ca                	sw	s2,80(sp)
500011c8:	c6ce                	sw	s3,76(sp)
500011ca:	c4d2                	sw	s4,72(sp)
500011cc:	c2d6                	sw	s5,68(sp)
500011ce:	c0da                	sw	s6,64(sp)
500011d0:	de5e                	sw	s7,60(sp)
500011d2:	dc62                	sw	s8,56(sp)
500011d4:	da66                	sw	s9,52(sp)
500011d6:	d86a                	sw	s10,48(sp)
500011d8:	d66e                	sw	s11,44(sp)
500011da:	c63a                	sw	a4,12(sp)
500011dc:	e28d                	bnez	a3,500011fe <lp_core_i2c_master_read_from_device+0x40>
500011de:	4501                	li	a0,0
500011e0:	40f6                	lw	ra,92(sp)
500011e2:	4466                	lw	s0,88(sp)
500011e4:	44d6                	lw	s1,84(sp)
500011e6:	4946                	lw	s2,80(sp)
500011e8:	49b6                	lw	s3,76(sp)
500011ea:	4a26                	lw	s4,72(sp)
500011ec:	4a96                	lw	s5,68(sp)
500011ee:	4b06                	lw	s6,64(sp)
500011f0:	5bf2                	lw	s7,60(sp)
500011f2:	5c62                	lw	s8,56(sp)
500011f4:	5cd2                	lw	s9,52(sp)
500011f6:	5d42                	lw	s10,48(sp)
500011f8:	5db2                	lw	s11,44(sp)
500011fa:	6125                	addi	sp,sp,96
500011fc:	8082                	ret
500011fe:	0ff00713          	li	a4,255
50001202:	8436                	mv	s0,a3
50001204:	10400513          	li	a0,260
50001208:	fcd76ce3          	bltu	a4,a3,500011e0 <lp_core_i2c_master_read_from_device+0x22>
5000120c:	50001b37          	lui	s6,0x50001
50001210:	5a0b2483          	lw	s1,1440(s6) # 500015a0 <dev>
50001214:	87ae                	mv	a5,a1
50001216:	670d                	lui	a4,0x3
50001218:	ccb8                	sw	a4,88(s1)
5000121a:	853e                	mv	a0,a5
5000121c:	8a32                	mv	s4,a2
5000121e:	4585                	li	a1,1
50001220:	01b10613          	addi	a2,sp,27
50001224:	37a5                	jal	5000118c <lp_core_i2c_config_device_addr.constprop.0>
50001226:	549c                	lw	a5,40(s1)
50001228:	6c05                	lui	s8,0x1
5000122a:	4981                	li	s3,0
5000122c:	0887e793          	ori	a5,a5,136
50001230:	d49c                	sw	a5,40(s1)
50001232:	4d41                	li	s10,16
50001234:	4489                	li	s1,2
50001236:	4b85                	li	s7,1
50001238:	800c0c13          	addi	s8,s8,-2048 # 800 <RvExcFrameSize+0x76c>
5000123c:	8922                	mv	s2,s0
5000123e:	008d5363          	bge	s10,s0,50001244 <lp_core_i2c_master_read_from_device+0x86>
50001242:	4941                	li	s2,16
50001244:	41240cb3          	sub	s9,s0,s2
50001248:	87a2                	mv	a5,s0
5000124a:	00148d93          	addi	s11,s1,1
5000124e:	8466                	mv	s0,s9
50001250:	07779063          	bne	a5,s7,500012b0 <lp_core_i2c_master_read_from_device+0xf2>
50001254:	8526                	mv	a0,s1
50001256:	875e                	mv	a4,s7
50001258:	4681                	li	a3,0
5000125a:	865e                	mv	a2,s7
5000125c:	458d                	li	a1,3
5000125e:	3719                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
50001260:	0489                	addi	s1,s1,2
50001262:	4701                	li	a4,0
50001264:	4681                	li	a3,0
50001266:	4601                	li	a2,0
50001268:	4589                	li	a1,2
5000126a:	856e                	mv	a0,s11
5000126c:	3de5                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
5000126e:	5a0b2783          	lw	a5,1440(s6)
50001272:	08800513          	li	a0,136
50001276:	43d8                	lw	a4,4(a5)
50001278:	01876733          	or	a4,a4,s8
5000127c:	c3d8                	sw	a4,4(a5)
5000127e:	43d8                	lw	a4,4(a5)
50001280:	02076713          	ori	a4,a4,32
50001284:	c3d8                	sw	a4,4(a5)
50001286:	45b2                	lw	a1,12(sp)
50001288:	359d                	jal	500010ee <lp_core_i2c_wait_for_interrupt>
5000128a:	f939                	bnez	a0,500011e0 <lp_core_i2c_master_read_from_device+0x22>
5000128c:	5a0b2683          	lw	a3,1440(s6)
50001290:	4edc                	lw	a5,28(a3)
50001292:	ce3e                	sw	a5,28(sp)
50001294:	01c14703          	lbu	a4,28(sp)
50001298:	013507b3          	add	a5,a0,s3
5000129c:	97d2                	add	a5,a5,s4
5000129e:	00e78023          	sb	a4,0(a5)
500012a2:	0505                	addi	a0,a0,1
500012a4:	fea916e3          	bne	s2,a0,50001290 <lp_core_i2c_master_read_from_device+0xd2>
500012a8:	99ca                	add	s3,s3,s2
500012aa:	f99049e3          	bgtz	s9,5000123c <lp_core_i2c_master_read_from_device+0x7e>
500012ae:	bf05                	j	500011de <lp_core_i2c_master_read_from_device+0x20>
500012b0:	0ff97713          	zext.b	a4,s2
500012b4:	020c9963          	bnez	s9,500012e6 <lp_core_i2c_master_read_from_device+0x128>
500012b8:	177d                	addi	a4,a4,-1 # 2fff <RvExcFrameSize+0x2f6b>
500012ba:	8526                	mv	a0,s1
500012bc:	0ff77713          	zext.b	a4,a4
500012c0:	4681                	li	a3,0
500012c2:	4601                	li	a2,0
500012c4:	458d                	li	a1,3
500012c6:	3d79                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
500012c8:	4705                	li	a4,1
500012ca:	863a                	mv	a2,a4
500012cc:	4681                	li	a3,0
500012ce:	458d                	li	a1,3
500012d0:	856e                	mv	a0,s11
500012d2:	3d49                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
500012d4:	00248a93          	addi	s5,s1,2
500012d8:	4701                	li	a4,0
500012da:	4681                	li	a3,0
500012dc:	4601                	li	a2,0
500012de:	4589                	li	a1,2
500012e0:	8556                	mv	a0,s5
500012e2:	048d                	addi	s1,s1,3
500012e4:	b761                	j	5000126c <lp_core_i2c_master_read_from_device+0xae>
500012e6:	8526                	mv	a0,s1
500012e8:	4681                	li	a3,0
500012ea:	4601                	li	a2,0
500012ec:	458d                	li	a1,3
500012ee:	3d9d                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
500012f0:	4701                	li	a4,0
500012f2:	4681                	li	a3,0
500012f4:	4601                	li	a2,0
500012f6:	4591                	li	a1,4
500012f8:	856e                	mv	a0,s11
500012fa:	35ad                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
500012fc:	4481                	li	s1,0
500012fe:	bf85                	j	5000126e <lp_core_i2c_master_read_from_device+0xb0>

50001300 <lp_core_i2c_master_write_to_device>:
50001300:	14068963          	beqz	a3,50001452 <lp_core_i2c_master_write_to_device+0x152>
50001304:	715d                	addi	sp,sp,-80
50001306:	c0ca                	sw	s2,64(sp)
50001308:	c686                	sw	ra,76(sp)
5000130a:	c4a2                	sw	s0,72(sp)
5000130c:	c2a6                	sw	s1,68(sp)
5000130e:	de4e                	sw	s3,60(sp)
50001310:	dc52                	sw	s4,56(sp)
50001312:	da56                	sw	s5,52(sp)
50001314:	d85a                	sw	s6,48(sp)
50001316:	d65e                	sw	s7,44(sp)
50001318:	d462                	sw	s8,40(sp)
5000131a:	d266                	sw	s9,36(sp)
5000131c:	d06a                	sw	s10,32(sp)
5000131e:	ce6e                	sw	s11,28(sp)
50001320:	0ff00793          	li	a5,255
50001324:	8936                	mv	s2,a3
50001326:	10400513          	li	a0,260
5000132a:	0ed7e463          	bltu	a5,a3,50001412 <lp_core_i2c_master_write_to_device+0x112>
5000132e:	50001bb7          	lui	s7,0x50001
50001332:	5a0ba403          	lw	s0,1440(s7) # 500015a0 <dev>
50001336:	882e                	mv	a6,a1
50001338:	8ab2                	mv	s5,a2
5000133a:	441c                	lw	a5,8(s0)
5000133c:	8b3a                	mv	s6,a4
5000133e:	8391                	srli	a5,a5,0x4
50001340:	8b85                	andi	a5,a5,1
50001342:	c789                	beqz	a5,5000134c <lp_core_i2c_master_write_to_device+0x4c>
50001344:	405c                	lw	a5,4(s0)
50001346:	4007e793          	ori	a5,a5,1024
5000134a:	c05c                	sw	a5,4(s0)
5000134c:	4c1c                	lw	a5,24(s0)
5000134e:	6709                	lui	a4,0x2
50001350:	00f10613          	addi	a2,sp,15
50001354:	8fd9                	or	a5,a5,a4
50001356:	cc1c                	sw	a5,24(s0)
50001358:	4c1c                	lw	a5,24(s0)
5000135a:	7779                	lui	a4,0xffffe
5000135c:	177d                	addi	a4,a4,-1 # ffffdfff <LPPERI+0x9ff4b7ff>
5000135e:	8ff9                	and	a5,a5,a4
50001360:	cc1c                	sw	a5,24(s0)
50001362:	4c1c                	lw	a5,24(s0)
50001364:	6705                	lui	a4,0x1
50001366:	4581                	li	a1,0
50001368:	8fd9                	or	a5,a5,a4
5000136a:	cc1c                	sw	a5,24(s0)
5000136c:	4c1c                	lw	a5,24(s0)
5000136e:	777d                	lui	a4,0xfffff
50001370:	177d                	addi	a4,a4,-1 # ffffefff <LPPERI+0x9ff4c7ff>
50001372:	8ff9                	and	a5,a5,a4
50001374:	cc1c                	sw	a5,24(s0)
50001376:	678d                	lui	a5,0x3
50001378:	cc3c                	sw	a5,88(s0)
5000137a:	8542                	mv	a0,a6
5000137c:	50001c37          	lui	s8,0x50001
50001380:	3531                	jal	5000118c <lp_core_i2c_config_device_addr.constprop.0>
50001382:	59cc4483          	lbu	s1,1436(s8) # 5000159c <s_ack_check_en>
50001386:	541c                	lw	a5,40(s0)
50001388:	00f14703          	lbu	a4,15(sp)
5000138c:	009034b3          	snez	s1,s1
50001390:	04aa                	slli	s1,s1,0xa
50001392:	08848493          	addi	s1,s1,136
50001396:	8fc5                	or	a5,a5,s1
50001398:	d41c                	sw	a5,40(s0)
5000139a:	6c85                	lui	s9,0x1
5000139c:	47c1                	li	a5,16
5000139e:	8f99                	sub	a5,a5,a4
500013a0:	844a                	mv	s0,s2
500013a2:	4a01                	li	s4,0
500013a4:	4989                	li	s3,2
500013a6:	800c8c93          	addi	s9,s9,-2048 # 800 <RvExcFrameSize+0x76c>
500013aa:	8922                	mv	s2,s0
500013ac:	0087f363          	bgeu	a5,s0,500013b2 <lp_core_i2c_master_write_to_device+0xb2>
500013b0:	893e                	mv	s2,a5
500013b2:	5a0bad03          	lw	s10,1440(s7)
500013b6:	41240433          	sub	s0,s0,s2
500013ba:	4781                	li	a5,0
500013bc:	0ff97693          	zext.b	a3,s2
500013c0:	06d7c863          	blt	a5,a3,50001430 <lp_core_i2c_master_write_to_device+0x130>
500013c4:	59cc4683          	lbu	a3,1436(s8)
500013c8:	0ff97713          	zext.b	a4,s2
500013cc:	4601                	li	a2,0
500013ce:	4585                	li	a1,1
500013d0:	854e                	mv	a0,s3
500013d2:	00198d93          	addi	s11,s3,1
500013d6:	3379                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
500013d8:	e42d                	bnez	s0,50001442 <lp_core_i2c_master_write_to_device+0x142>
500013da:	4701                	li	a4,0
500013dc:	4681                	li	a3,0
500013de:	4601                	li	a2,0
500013e0:	4589                	li	a1,2
500013e2:	856e                	mv	a0,s11
500013e4:	0989                	addi	s3,s3,2
500013e6:	3bbd                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
500013e8:	004d2783          	lw	a5,4(s10)
500013ec:	85da                	mv	a1,s6
500013ee:	8526                	mv	a0,s1
500013f0:	0197e7b3          	or	a5,a5,s9
500013f4:	00fd2223          	sw	a5,4(s10)
500013f8:	004d2783          	lw	a5,4(s10)
500013fc:	0207e793          	ori	a5,a5,32
50001400:	00fd2223          	sw	a5,4(s10)
50001404:	31ed                	jal	500010ee <lp_core_i2c_wait_for_interrupt>
50001406:	e511                	bnez	a0,50001412 <lp_core_i2c_master_write_to_device+0x112>
50001408:	9a4a                	add	s4,s4,s2
5000140a:	47c1                	li	a5,16
5000140c:	f8804fe3          	bgtz	s0,500013aa <lp_core_i2c_master_write_to_device+0xaa>
50001410:	4501                	li	a0,0
50001412:	40b6                	lw	ra,76(sp)
50001414:	4426                	lw	s0,72(sp)
50001416:	4496                	lw	s1,68(sp)
50001418:	4906                	lw	s2,64(sp)
5000141a:	59f2                	lw	s3,60(sp)
5000141c:	5a62                	lw	s4,56(sp)
5000141e:	5ad2                	lw	s5,52(sp)
50001420:	5b42                	lw	s6,48(sp)
50001422:	5bb2                	lw	s7,44(sp)
50001424:	5c22                	lw	s8,40(sp)
50001426:	5c92                	lw	s9,36(sp)
50001428:	5d02                	lw	s10,32(sp)
5000142a:	4df2                	lw	s11,28(sp)
5000142c:	6161                	addi	sp,sp,80
5000142e:	8082                	ret
50001430:	01478733          	add	a4,a5,s4
50001434:	9756                	add	a4,a4,s5
50001436:	00074703          	lbu	a4,0(a4)
5000143a:	0785                	addi	a5,a5,1 # 3001 <RvExcFrameSize+0x2f6d>
5000143c:	00ed2e23          	sw	a4,28(s10)
50001440:	b741                	j	500013c0 <lp_core_i2c_master_write_to_device+0xc0>
50001442:	4701                	li	a4,0
50001444:	4681                	li	a3,0
50001446:	4601                	li	a2,0
50001448:	4591                	li	a1,4
5000144a:	856e                	mv	a0,s11
5000144c:	3b21                	jal	50001164 <lp_core_i2c_format_cmd.constprop.0>
5000144e:	4981                	li	s3,0
50001450:	bf61                	j	500013e8 <lp_core_i2c_master_write_to_device+0xe8>
50001452:	4501                	li	a0,0
50001454:	8082                	ret
