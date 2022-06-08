from utime import ticks_ms,ticks_diff
from .  import simple2
class MQTTClient(simple2.MQTTClient):
	DEBUG=False;KEEP_QOS0=True;NO_QUEUE_DUPS=True;MSG_QUEUE_MAX=5;CONFIRM_QUEUE_MAX=10;RESUBSCRIBE=True
	def __init__(A,*B,**C):super().__init__(*B,**C);A.subs=[];A.msg_to_send=[];A.sub_to_send=[];A.msg_to_confirm={};A.sub_to_confirm={};A.conn_issue=None
	def is_keepalive(A):
		B=ticks_diff(ticks_ms(),A.last_cpacket)//1000
		if 0<A.keepalive<B:A.conn_issue=simple2.MQTTException(7),9;return False
		return True
	def set_callback_status(A,f):A._cbstat=f
	def cbstat(A,pid,stat):
		E=stat;D=pid
		try:A._cbstat(D,E)
		except AttributeError:pass
		for (B,C) in A.msg_to_confirm.items():
			if D in C:
				if E==0:
					if B not in A.msg_to_send:A.msg_to_send.insert(0,B)
					C.remove(D)
					if not C:A.msg_to_confirm.pop(B)
				elif E in(1,2):A.msg_to_confirm.pop(B)
				return
		for (B,C) in A.sub_to_confirm.items():
			if D in C:
				if E==0:
					if B not in A.sub_to_send:A.sub_to_send.append(B)
					C.remove(D)
					if not C:A.sub_to_confirm.pop(B)
				elif E in(1,2):A.sub_to_confirm.pop(B)
	def connect(A,clean_session=True):
		B=clean_session
		if B:A.msg_to_send[:]=[];A.msg_to_confirm.clear()
		try:C=super().connect(B);A.conn_issue=None;return C
		except (OSError,simple2.MQTTException)as D:A.conn_issue=D,1
	def log(A):
		if A.DEBUG:
			if type(A.conn_issue)is tuple:B,C=A.conn_issue
			else:B=A.conn_issue;C=0
			D='?','connect','publish','subscribe','reconnect','sendqueue','disconnect','ping','wait_msg','keepalive','check_msg';print('MQTT (%s): %r'%(D[C],B))
	def reconnect(A):
		try:B=super().connect(False);A.conn_issue=None;return B
		except (OSError,simple2.MQTTException)as C:
			A.conn_issue=C,4
			if A.sock:A.sock.close();A.sock=None
	def resubscribe(A):
		for (B,C) in A.subs:A.subscribe(B,C,False)
	def add_msg_to_send(A,data):
		C=len(A.msg_to_send);C+=sum(map(len,A.msg_to_confirm.values()))
		while C>=A.MSG_QUEUE_MAX:
			E=min(map(lambda x:x[0]if x else 65535,A.msg_to_confirm.values()),default=0)
			if 0<E<65535:
				B=None
				for (F,D) in A.msg_to_confirm.items():
					if D and D[0]==E:del D[0];B=F;break
				if B and B in A.msg_to_confirm and not A.msg_to_confirm[B]:A.msg_to_confirm.pop(B)
			else:A.msg_to_send.pop(0)
			C-=1
		A.msg_to_send.append(data)
	def disconnect(A):
		try:return super().disconnect()
		except (OSError,simple2.MQTTException)as B:A.conn_issue=B,6
	def ping(A):
		if not A.is_keepalive():return
		try:return super().ping()
		except (OSError,simple2.MQTTException)as B:A.conn_issue=B,7
	def publish(A,topic,msg,retain=False,qos=0):
		E=topic;D=retain;B=qos;C=E,msg,D,B
		if D:A.msg_to_send[:]=[B for B in A.msg_to_send if not(E==B[0]and D==B[2])]
		try:
			F=super().publish(E,msg,D,B,False)
			if B==1:
				A.msg_to_confirm.setdefault(C,[]).append(F)
				if len(A.msg_to_confirm[C])>A.CONFIRM_QUEUE_MAX:A.msg_to_confirm.pop(0)
			return F
		except (OSError,simple2.MQTTException)as G:
			A.conn_issue=G,2
			if A.NO_QUEUE_DUPS:
				if C in A.msg_to_send:return
			if A.KEEP_QOS0 and B==0:A.add_msg_to_send(C)
			elif B==1:A.add_msg_to_send(C)
	def subscribe(A,topic,qos=0,resubscribe=True):
		C=topic;B=C,qos
		if A.RESUBSCRIBE and resubscribe:
			if C not in dict(A.subs):A.subs.append(B)
		A.sub_to_send[:]=[B for B in A.sub_to_send if C!=B[0]]
		try:
			D=super().subscribe(C,qos);A.sub_to_confirm.setdefault(B,[]).append(D)
			if len(A.sub_to_confirm[B])>A.CONFIRM_QUEUE_MAX:A.sub_to_confirm.pop(0)
			return D
		except (OSError,simple2.MQTTException)as E:
			A.conn_issue=E,3
			if A.NO_QUEUE_DUPS:
				if B in A.sub_to_send:return
			A.sub_to_send.append(B)
	def send_queue(A):
		D=[]
		for B in A.msg_to_send:
			E,I,J,C=B
			try:
				F=super().publish(E,I,J,C,False)
				if C==1:A.msg_to_confirm.setdefault(B,[]).append(F)
				D.append(B)
			except (OSError,simple2.MQTTException)as G:A.conn_issue=G,5;return False
		A.msg_to_send[:]=[B for B in A.msg_to_send if B not in D];del D;H=[]
		for B in A.sub_to_send:
			E,C=B
			try:F=super().subscribe(E,C);A.sub_to_confirm.setdefault(B,[]).append(F);H.append(B)
			except (OSError,simple2.MQTTException)as G:A.conn_issue=G,5;return False
		A.sub_to_send[:]=[B for B in A.sub_to_send if B not in H];return True
	def is_conn_issue(A):
		A.is_keepalive()
		if A.conn_issue:A.log()
		return bool(A.conn_issue)
	def wait_msg(A):
		A.is_keepalive()
		try:return super().wait_msg()
		except (OSError,simple2.MQTTException)as B:A.conn_issue=B,8
	def check_msg(A):
		A.is_keepalive()
		try:return super().check_msg()
		except (OSError,simple2.MQTTException)as B:A.conn_issue=B,10