/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package wisielec;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import static wisielec.Wisielec.clientSocket;
import static wisielec.Wisielec.reader;

/**
 * FXML Controller class
 *
 * @author wisni
 */
public class FXMLController implements Initializable {

    @FXML
    private TextField TextLabel;
    @FXML
    private Button Button;
    @FXML
    private Button WyslijButton;

    /**
     * Initializes the controller class.
     */
    @FXML
    private void MainButton(ActionEvent event) throws IOException {
        //String serverMessage = "";
        //InputStream is = clientSocket.getInputStream();
        //byte[] buffer = new byte[100];
        //is.read(buffer);

        
        //serverMessage = reader.readLine();
        String inputLine;
        inputLine = reader.readLine();
        //while ((inputLine = reader.readLine()) != null) {
            System.out.println("Wiadomość: " + inputLine);
        //}

        //while (reader != null) {
        //    String serverMessage = reader.readLine();
        //    System.out.println("Wiadomość: " + serverMessage);
        //}
        //BufferedReader reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        //StringBuilder sb = new StringBuilder();
        //String line;
        //while ((line = reader.readLine()) != null) {
        //    sb.append(line).append("\n");
        //}
    }

    @Override
    public void initialize(URL url, ResourceBundle rb) {
        //TO DO
    }

    @FXML
    private void WyslijAction(ActionEvent event) throws IOException {
        String clientMessage = TextLabel.getText();
        PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
        writer.println(clientMessage);
    }

}
