import java.io.DataInputStream;
import java.io.FileInputStream;
import java.util.Date;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import javax.jms.*;
import javax.naming.*;

public class QueueExample {

    private static final int THREAD_POOL_SIZE = 10;

    public static void main(String[] args) throws Exception {
        // Establecer la conexión con el servidor JMS
        Context context = new InitialContext();
        QueueConnectionFactory qconFactory = (QueueConnectionFactory) context.lookup("ConnectionFactory");
        QueueConnection qcon = qconFactory.createQueueConnection();
        qcon.start();

        // Crear una sesión y una cola
        QueueSession session = qcon.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);
        Queue queue = (Queue) context.lookup("queue/TestQueue");

        // Crear un QueueSender y un QueueReceiver
        QueueSender sender = session.createSender(queue);
        QueueReceiver receiver = session.createReceiver(queue);

        // Leer los datos desde el archivo .dat y enviarlos al QueueSender utilizando varios threads
        ExecutorService executorService = Executors.newFixedThreadPool(THREAD_POOL_SIZE);
        DataInputStream dis = new DataInputStream(new FileInputStream("datos.dat"));
        int data;
        while ((data = dis.readInt()) != -1) {
            final int finalData = data;
            executorService.execute(() -> {
                try {
                    // Medir el tiempo de procesamiento
                    long startTime = System.currentTimeMillis();

                    // Enviar el dato al QueueSender
                    TextMessage message = session.createTextMessage(Integer.toString(finalData));
                    sender.send(message);

                    // Recibir la respuesta correspondiente desde el QueueReceiver
                    TextMessage response = (TextMessage) receiver.receive();

                    // Imprimir la respuesta y el tiempo de procesamiento
                    System.out.println("Respuesta recibida: " + response.getText());
                    long endTime = System.currentTimeMillis();
                    System.out.println("Tiempo de procesamiento: " + (endTime - startTime) + "ms");
                } catch (Exception e) {
                    e.printStackTrace();
                }
            });
        }

        // Esperar a que todos los threads hayan terminado antes de enviar el mensaje de quit al QueueReceiver
        executorService.shutdown();
        while (!executorService.isTerminated()) {
            Thread.sleep(1000);
        }

        // Enviar un mensaje de quit al QueueReceiver para detenerlo
        TextMessage quitMessage = session.createTextMessage("quit");
        sender.send(quitMessage);

        // Cerrar la conexión con el servidor JMS
        qcon.close();
    }
}
