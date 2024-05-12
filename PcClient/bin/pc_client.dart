import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';
import 'package:web_socket_client/web_socket_client.dart';


void main(List<String> arguments) async {
  final wsUrl = Uri.parse('ws://10.0.0.175/ws');
  // final channel = WebSocketChannel.connect(wsUrl);
  final socket = WebSocket(wsUrl);

  final p = await Process.start("cava", ["-p", "/home/simrat39/cava.conf"]);

  const bars = 32;
  const bytesize = 2;
  const bytenorm = 65535;
  final chunksize = bytesize * bars;

  await for (final dataChunk in p.stdout) {
    if (dataChunk.length != chunksize) continue;

    final data = Uint8List.fromList(dataChunk).buffer.asUint16List(0, bars).map((e) => e * (8.0 / 65535.0)).map((e) => e.floor()).toList();
    if (data.length != 32) {
      continue;
    }
    socket.send(data);
    // channel.sink.add(data);
  }

  socket.close();
}
